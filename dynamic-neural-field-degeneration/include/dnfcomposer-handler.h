#pragma once

#include <thread>
#include <iostream>

#include "../lib/dynamic-neural-field-composer/include/application/application.h"
#include "./dnf-architecture.h"
#include "./macros.h"
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
	const int timeForFieldToSettle = 20;
	ElementDegeneracyType degeneracyType = ElementDegeneracyType::NONE;
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

	bool wasIntializationRequested = false;
	bool wasExternalInputUpdated = false;
	bool wasDegenerationRequested = false;
	bool haveFieldsSettled = false;

public:
	DnfcomposerHandler();
	~DnfcomposerHandler() = default;
	
	void init();
	void step();
	void close();


	void setDegeneracy(ElementDegeneracyType degeneracyType);
	void setExternalInput(const double& position);
	void setHaveFieldsSettled(bool haveFieldsSettled);
	void setIsUserInterfaceActiveAs(bool isUserInterfaceActive) const;
	void setInitializeFields();

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