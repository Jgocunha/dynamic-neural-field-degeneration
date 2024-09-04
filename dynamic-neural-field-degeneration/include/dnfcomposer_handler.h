#pragma once

#include <thread>
#include <iostream>
#include <elements/field_coupling.h>
#include <elements/neural_field.h>

#include "application/application.h"
#include "dnf_architecture.h"
#include "user_interface_window.h"
#include "degenerate_field_coupling.h"
#include "imgui-platform-kit/log_window.h"
#include "user_interface/plot_window.h"
#include "user_interface/simulation_window.h"
#include "user_interface/main_window.h"
#include "dnf_architecture.h"
#include "application/application.h"
#include <degenerate_neural_field.h>


struct SimulationElements
{
	std::shared_ptr<DegenerateNeuralField> inputField, outputField;
	std::shared_ptr<DegenerateFieldCoupling> fieldCoupling;
};

struct SimulationParameters
{
	std::string inputFieldId = "perceptual field";
	std::string outputFieldId = "output field";
	std::string fieldCouplingId = "per - out";
	double externalInputPosition = 0;
	double inputFieldCentroid, outputFieldCentroid;
	int timeForFieldToSettle = 25;
	ElementDegeneracyType degeneracyType = ElementDegeneracyType::NONE;
	std::string fieldToDegenerate = "perceptual";

	bool isDebugMode = false;
	bool isUserInterfaceActive = false;
};

class DnfcomposerHandler
{
private:
	std::thread dnfcomposerThread;
	std::thread readCentroidsThread;
	std::thread applicationThread;

	std::unique_ptr<dnf_composer::Application> application;
	std::shared_ptr<dnf_composer::Simulation> simulation;
	std::shared_ptr<ExperimentWindow> userInterfaceWindow;

	SimulationElements simulationElements;
	SimulationParameters simulationParameters;

	int numberOfDegeneratedElements = 0;
	int numberOfElementsToDegenerate = 0;

	bool wasIntializationRequested = false;
	bool wasExternalInputUpdated = false;
	bool wasDegenerationRequested = false;
	bool haveFieldsSettled = false;
	bool hasTrialFinished = false;
	bool hasExperimentFinished = false;
	bool isCentroidDataBeingAccessed = false;

public:
	DnfcomposerHandler();
	DnfcomposerHandler(bool isUserInterfaceActive);

	~DnfcomposerHandler() = default;
	
	void init();
	void step();
	void close();
	void stop();

	void closeSimulation();

	void setExperimentSetupData(const std::string& currentDegenerationType,
		const double& maximumAllowedDeviation, const std::string& typeOfElementsDegenerated) const;
	void setExpectedFieldBehavior(const double& targetPerceptualFieldCentroid, const double& targetDecisionFieldCentroid) const;
	void setTrial(const int& trial) const;
	void setExternalInput(const double& position);
	void setDegeneracy(ElementDegeneracyType degeneracyType, const std::string& fieldToDegenerate);
	void setHaveFieldsSettled(bool haveFieldsSettled);
	void setIsUserInterfaceActiveAs(bool isUserInterfaceActive) const;
	void setCentroidDataBeingAccessed(bool isCentroidDataBeingAccessed);
	void setNumberOfElementsToDegenerate(const int& numberOfElementsToDegenerate);

	double getInputFieldCentroid() const;
	double getOutputFieldCentroid() const;
	bool getHaveFieldsSettled() const;
	std::shared_ptr<ExperimentWindow> getUserInterfaceWindow();
	
private:
	void initializeFields();
	void setupUserInterface();
	void cleanUpTrial();
	void updateExternalInput();
	void updateFieldCentroids();
	void activateDegeneration();
	void waitForFieldsToSettle() const;
};