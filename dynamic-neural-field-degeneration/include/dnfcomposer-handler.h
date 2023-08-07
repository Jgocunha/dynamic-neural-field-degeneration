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
};

class DnfcomposerHandler
{
private:
	std::thread dnfcomposerThread;
	std::unique_ptr<Application> application;
	std::shared_ptr<Simulation> simulation;
	std::shared_ptr<ExperimentWindow> userInterfaceWindow;

	SimulationElements simulationElements;
	SimulationParameters simulationParameters;

	bool wasExternalInputUpdated = false;
	bool haveFieldsSettled = false;

	std::unordered_map<double, int> hueToAngleMap = {
		{00.00,  15},
		{40.6034, 40},
		{60.00,  65},
		{120.00, 90},
		{240.00, 115},
		{274.146, 140},
		{284.791, 165}
	};

public:
	DnfcomposerHandler();
	~DnfcomposerHandler() = default;
	
	void init();
	void step();
	void close();

	void setExternalInput(const double& position);
	double getInputFieldCentroid();
	double getOutputFieldCentroid();
	bool getHaveFieldsSettled();
private:
	void setupUserInterface();

	void updateExternalInput();
	void updateFieldCentroids();

	void updateUserInterface();
};