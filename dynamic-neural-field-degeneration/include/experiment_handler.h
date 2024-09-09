
#pragma once

#include <iostream>
#include <thread>
#include <iomanip>

#include "dnfcomposer_handler.h"
#include "experiment_parameters.h"


//struct ExperimentParameters
//{
//	std::string filePathPrefix = "../../../data/";
//	
//	double decisionTolerance = 5;
//	int numberOfTrials = 10;
//
//	ElementDegeneracyType degeneracyType = ElementDegeneracyType::WEIGHTS_DEACTIVATE;
//	std::string degeneracyName = "deactivate-weights";
//	std::string typeOfElementsDegenerated = "weights";
//	std::string fieldToDegenerate = "perceptual";
//
//	int startingExternalStimulus = 0;
//	int initialPercentageOfDegeneration = 0;
//	int targetPercentageOfDegeneration = 100;
//	int currentPercentageOfDegeneration = 0;
//	int numberOfElementsToDegeneratePerIteration = 0;
//	int totalNumberOfElementsToDegenerate = 0;
//	int currentTrial = 0;
//
//	bool isDataSavingOn = false;
//	bool isVisualisationOn = true;
//	bool isDebugModeOn = true;
//};

struct ExperimentData
{
	double inputFieldCentroid = -1;
	double outputFieldCentroid = -1;
	double targetInputFieldCentroid = -1;
	double targetOutputFieldCentroid = -1;
	std::vector<double> outputFieldCentroidHistory;
};

class ExperimentHandler
{
private:
	DnfcomposerHandler dnfcomposerHandler;
	std::thread experimentThread;

	ExperimentParameters params;
	ExperimentData data;

	std::unordered_map<double, int> hueToAngleMap = {
		{00.00,  78.00}, // red
		{41.00,  82.00}, // blue
		{60.00,  86.00}, // yellow
		{120.00, 90.00}, // green
		{240.00, 94.00}, // orange
		{274.00, 98.00}, // indigo
		{300.00, 102.00} // violet
	};


	std::unordered_map<double, int>::iterator hueToAngleIterator = hueToAngleMap.begin();

public:
	ExperimentHandler();
	~ExperimentHandler() = default;

	void init();
	void step();
	void close();

private:
	void printExperimentSetupToConsole();
	void setExpectedFieldBehaviour();
	void setExperimentAsEnded();
	void setExperimentSetupData();

	void setupProcedure();
	void degenerationProcedure();
	void cleanUpTrial();

	bool hasOutputFieldDegenerated() ;
	void saveOutputFieldCentroidToFile() const;
};