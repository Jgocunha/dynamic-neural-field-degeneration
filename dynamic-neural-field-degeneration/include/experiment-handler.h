
#pragma once

#include <iostream>
#include <thread>

#include "./dnfcomposer-handler.h"
#include "macros.h"


//std::unordered_map<double, int> hueToAngleMap = {
//		{00.00,  15},
//		{40.60,  40},
//		{60.00,  65},
//		{120.00, 90},
//		{240.00, 115},
//		{274.00, 140},
//		{282.00, 165}
//};

struct ExperimentParameters
{
	std::string filePathPrefix = "../../../data/";
	
	double decisionTolerance = 5;
	int numberOfTrials = 10;

	ElementDegeneracyType degeneracyType = ElementDegeneracyType::WEIGHTS_DEACTIVATE;
	std::string degeneracyName = "deactivate-weights";
	std::string typeOfElementsDegenerated = "weights";
	std::string fieldToDegenerate = "perceptual";

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
	void setExperimentAsEnded();

	void setupProcedure(const int& trial);
	void degenerationProcedure();
	void cleanUpTrial();

	bool hasOutputFieldDegenerated() const;
	void saveOutputFieldCentroidToFile() const;
};