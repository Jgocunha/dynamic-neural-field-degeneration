
#pragma once

#include <iostream>
#include <thread>

#include "./dnfcomposer-handler.h"
#include "macros.h"


struct ExperimentParameters
{
	std::string filePathPrefix = "../../../data/";
	
	double decisionTolerance = 5;
	int numberOfTrials = 10;

	ElementDegeneracyType degeneracyType = ElementDegeneracyType::WEIGHTS_DEACTIVATE;
	std::string degeneracyName = "deactivate-weights";
	std::string typeOfElementsDegenerated = "weights";
	std::string fieldToDegenerate = "perceptual";

	int startingExternalStimulus = 0;
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
	double targetInputFieldCentroid = UNDEFINED;
	double targetOutputFieldCentroid = UNDEFINED;
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
		//{00.00,  15}, // red
		//{41.00,  40}, // blue
		//{60.00,  65}, // yellow
		//{120.00, 90}, // green
		//{240.00, 115}, // orange
		{274.00, 140}, // indigo
		//{282.00, 165} // violet
	};
	std::unordered_map<double, int>::iterator hueToAngleIterator = hueToAngleMap.begin();

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
	void setExpectedFieldBehaviour();
	void setExperimentAsEnded();

	void setupProcedure();
	void degenerationProcedure();
	void cleanUpTrial();

	bool hasOutputFieldDegenerated() ;
	void saveOutputFieldCentroidToFile() const;
};