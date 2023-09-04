
#pragma once

#include <iostream>
#include <thread>

#include "./dnfcomposer-handler.h"
#include "macros.h"

struct ExperimentParameters
{
	std::string filePathPrefix = "../../../data/";
	
	int decisionTolerance = 5;
	int numberOfTrials = 10;

	ElementDegeneracyType degeneracyType = ElementDegeneracyType::WEIGHTS_DEACTIVATE;
	std::string degeneracyName = "deactivate-weights";
	std::string typeOfElementsDegenerated = "weights";

	double targetExternalStimulusPosition = 0;
	double targetOutputCentroid = 20;

	int initialPercentageOfDegeneration = 0;
	int targetPercentageOfDegeneration = 100;
	int currentPercentageOfDegeneration = 0;

	bool isDataSavingOn = false;
	bool isVisualisationOn = true;
	bool isDebugModeOn = true;
};

struct ExperimentData
{
	double inputFieldCentroid = UNDEFINED;
	double outputFieldCentroid = UNDEFINED;
	double lastOutputFieldCentroid = UNDEFINED;
	std::vector<double> outputFieldCentroidHistory;
};

class ExperimentHandler
{
private:
	DnfcomposerHandler dnfcomposerHandler;
	std::thread experimentThread;

	ExperimentParameters params;
	ExperimentData data;

public:
	ExperimentHandler() = default;
	ExperimentHandler(const ExperimentParameters& params);
	~ExperimentHandler() = default;

	void init();
	void step();
	void close();

private:
	void printExperimentSetupToConsole() const;
	void setExperimentSetupData() const;
	void setExpectedFieldBehaviour() const;

	void setupProcedure(const int& trial);
	void degenerationProcedure();
	void cleanUpTrial();

	bool hasOutputFieldDegenerated() const;
	void saveOutputFieldCentroidToFile() const;
};