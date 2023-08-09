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
	double externalInputPosition, expectedOutputCentroid;
	double inputFieldCentroid, outputFieldCentroid;
	const int timeForFieldToSettle = 25;
	ElementDegeneracyType degeneracyType = ElementDegeneracyType::NONE;
};

struct RelearningParameters
{
	double expectedInputCentroid = 0.0, expectedOutputCentroid = 0.0;
};

class DnfcomposerHandler
{
private:
	std::thread dnfcomposerThread;
	std::thread readCentroidsThread;
	std::unique_ptr<Application> application;
	std::shared_ptr<Simulation> simulation;
	std::shared_ptr<ExperimentWindow> userInterfaceWindow;

	SimulationElements simulationElements;
	SimulationParameters simulationParameters;
	RelearningParameters relearningParameters;

	bool wasExternalInputUpdated = false;
	bool wasDegenerationRequested = false;
	bool wasRelearningRequested = false;
	bool haveFieldsSettled = false;
	bool hasRelearningFinished = false;

public:
	DnfcomposerHandler();
	~DnfcomposerHandler() = default;
	
	void init();
	void step();
	void close();

	void setDegeneracy(ElementDegeneracyType degeneracyType);
	void setExternalInput(const double& position);
	//void setRelearning();
	void setRelearning(const double& expectedInputCentroid, const double& expectedOutputCentroid);
	void setHaveFieldsSettled(bool haveFieldsSettled);

	double getInputFieldCentroid();
	double getOutputFieldCentroid();
	bool getHaveFieldsSettled();
	bool getHasRelearningFinished();
	std::shared_ptr<ExperimentWindow> getUserInterfaceWindow();
private:
	void setupUserInterface();

	void updateExternalInput();
	void updateFieldCentroids();

	void activateDegeneration();
	void activateRelearning();
};