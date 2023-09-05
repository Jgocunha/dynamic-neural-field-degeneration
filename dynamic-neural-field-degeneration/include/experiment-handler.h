
#pragma once

#include <iostream>
#include <thread>
#include <bitset>

#include "./coppeliasim-handler.h"
#include "./dnfcomposer-handler.h"


struct ExperimentParameters
{
	std::string filePathPrefix = "../../../data/";
	
	int numberOfShapesPerTrial = 2;
	double decisionTolerance = 0.5;
	int numberOfTrials = 10;

	ElementDegeneracyType degeneracyType = ElementDegeneracyType::WEIGHTS_DEACTIVATE;
	std::string degeneracyName = "deactivate-weights";
	std::string typeOfElementsDegenerated = "weights";
	std::string fieldToDegenerate = "perceptual";

	int initialPercentageOfDegeneration = 0;
	int targetPercentageOfDegeneration = 100;
	int currentPercentageOfDegeneration = 0;
	int incrementOfDegenerationPercentage = 10;

	int maximumAmountOfRelearningCycles = 10;

	bool isDataSavingOn = false;
	bool isVisualisationOn = true;
	bool isDebugModeOn = true;
};

struct ExperimentData
{
	double inputFieldCentroid = UNDEFINED;
	double outputFieldCentroid = UNDEFINED;
	double lastOutputFieldCentroid = UNDEFINED;
	double shapeHue = UNDEFINED;
	double expectedTargetAngle = UNDEFINED;
};

struct ExperimentStatistics
{
	int numDecisions = 0;
	int numCorrectDecisions = 0;
	int numIncorrectDecisions = 0;
	double decisionRatio = 0.0;
	int numOfRelearningCycles = 0;
	int shapesPlacedIncorrectly = 0;
};

class ExperimentHandler
{
private:
	CoppeliasimHandler coppeliasimHandler;
	DnfcomposerHandler dnfcomposerHandler;
	std::thread experimentThread;

	ExperimentParameters param;
	ExperimentData data;
	ExperimentStatistics stats;
	Signals signals;

	std::unordered_map<double, int> hueToAngleMap = {
		{00.00,  15},
		{40.60,  40},
		{60.00,  65},
		{120.00, 90},
		{240.00, 115},
		{274.00, 140},
		{282.00, 165}
	};

public:
	ExperimentHandler() = default;
	ExperimentHandler(const ExperimentParameters& param);
	~ExperimentHandler() = default;

	// init step close functions
	void init();
	void step();
	void close();

private:
	// ui setup functions
	void printExperimentSetupToConsole() const;
	void setExperimentSetupData() const;
	void setExpectedFieldBehaviour() const;
	void setExperimentAsEnded();
private:

	// control functions
	bool pickAndPlace();
	void createShape();
	void graspShape();
	void placeShape();
	bool verifyDecision();
	void readShapeHue();
	void readTargetAngle();

	// cleanup functions
	void cleanUpTrial();
	void updateStatistics();

	// relearning functions
	void relearningProcedure();
	// degeneration functions
	void degenerationProcedure();
	int computeNumberOfElementsToDegenerate() const;

	// data collection functions
	void copyWeightsFile(const std::string& newFilename = "per - dec_weights - copy.txt") const;
	void deleteBackupAndRenameWeightsFile() const;
	bool doesBackupWeightsFileExist() const;
	void saveLearningCyclesPerTrial() const;
	void saveNumberOfIncorrectlyPlacedBoxes() const;
};