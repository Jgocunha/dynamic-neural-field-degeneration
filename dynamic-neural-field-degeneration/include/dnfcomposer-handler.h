#pragma once

#include <thread>
#include <iostream>

#include "../lib/dynamic-neural-field-composer/include/application/application.h"
#include "./dnf-architecture.h"
#include "./user-interface-window.h"

struct SimulationElements
{
	std::shared_ptr<DegenerateNeuralField> inputField, outputField;
	std::shared_ptr<DegenerateFieldCoupling> fieldCoupling;
};

struct SimulationParameters
{
	std::string inputFieldId = "perceptual field";
	std::string outputFieldId = "decision field";
	std::string fieldCouplingId = "per - dec";
	double externalInputPosition = 0;
	double inputFieldCentroid, outputFieldCentroid;
	int timeForFieldToSettle = 5;
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

	std::unique_ptr<Application> application;
	std::shared_ptr<Simulation> simulation;
	std::shared_ptr<ExperimentWindow> userInterfaceWindow;

	SimulationElements simulationElements;
	SimulationParameters simulationParameters;

	int numberOfDegeneratedElements = 0;

	bool wasIntializationRequested = false;
	bool wasExternalInputUpdated = false;
	bool wasDegenerationRequested = false;
	bool haveFieldsSettled = false;
	bool hasExperimentFinished = false;

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

	double getInputFieldCentroid() const;
	double getOutputFieldCentroid() const;
	bool getHaveFieldsSettled() const;
	std::shared_ptr<ExperimentWindow> getUserInterfaceWindow();
	
private:
	void initializeFields();
	void setupUserInterface();
	void updateExternalInput();
	void updateFieldCentroids();
	void activateDegeneration();
	void waitForFieldsToSettle() const;
};