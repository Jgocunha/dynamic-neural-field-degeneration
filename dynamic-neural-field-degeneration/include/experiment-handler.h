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
	double decisionTolerance = 0;
	int numberOfTrials = 10;

	ElementDegeneracyType degeneracyType = ElementDegeneracyType::WEIGHTS_DEACTIVATE;
	std::string degeneracyName = "deactivate-weights";
	std::string typeOfElementsDegenerated = "weights";
	std::string fieldToDegenerate = "perceptual";

	int initialPercentageOfDegeneration = 0;
	int targetPercentageOfDegeneration = 100;
	int currentPercentageOfDegeneration = 0;
	int incrementOfDegenerationPercentage = 10;

	RelearningParameters::RelearningType relearningType = RelearningParameters::RelearningType::ALL_CASES;
	double learningRate = 0.1;
	int numberOfRelearningEpochs = 100;
	int maximumAmountOfDemonstrations = 10;
	bool updateAllWeights = true;

	bool isDataSavingOn = false;
	bool isComposerVisualizationOn = true;
	bool isDebugModeOn = true;
	bool isLinkToCoppeliaSimOn = true;

	void setOtherDegeneracyParameters()
	{
		switch (degeneracyType)
		{
		case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
			degeneracyName = "deactivate";
			typeOfElementsDegenerated = "weights";
			break;
		case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
			degeneracyName = "randomize";
			typeOfElementsDegenerated = "weights";
			break;
		case ElementDegeneracyType::WEIGHTS_REDUCE:
			degeneracyName = "reduce 0.4";
			typeOfElementsDegenerated = "weights";
			break;
		case ElementDegeneracyType::NEURONS_DEACTIVATE:
			if (fieldToDegenerate == "perceptual")
				typeOfElementsDegenerated = "pre-synaptic neurons";
			else if (fieldToDegenerate == "decision")
				typeOfElementsDegenerated = "post-synaptic neurons";
			degeneracyName = "deactivate";
			break;
		default:
			break;
		}
		degeneracyName = degeneracyName + " " + typeOfElementsDegenerated;
	}
	std::string getSavePath() const
	{
		std::string filename = filePathPrefix + "results/";

		filename = filename + degeneracyName;

		switch(relearningType)
		{
		case  RelearningParameters::RelearningType::ONLY_DEGENERATED_CASES:
			filename = filename + " Only-degenerated-cases ";
			break;
		case RelearningParameters::RelearningType::ALL_CASES:
			filename = filename + "All-cases ";
			break;
		}

		filename = filename + " Epochs-" + std::to_string(numberOfRelearningEpochs);
		filename = filename + " MaxCycles-" + std::to_string(maximumAmountOfDemonstrations);
		filename = filename + " Update-all-weights-" + std::to_string(updateAllWeights);
		filename = filename + ".txt";

		return filename;
	}
};

struct ExperimentData
{
	double inputFieldCentroid = UNDEFINED;
	double outputFieldCentroid = UNDEFINED;
	double lastOutputFieldCentroid = UNDEFINED;
	double shapeHue = UNDEFINED;
	double expectedTargetAngle = UNDEFINED;
	bool isFieldDead = false;
};

struct ExperimentStatistics
{
	int numOfRelearningCycles = 0;
	int shapesPlacedIncorrectly = 0;
	std::vector<int> learningCyclesPerTrialHistory;
};

class ExperimentHandler
{
private:
	std::thread experimentThread;
	CoppeliasimHandler coppeliasimHandler;
	DnfcomposerHandler dnfcomposerHandler;

	ExperimentParameters params;
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
	std::unordered_map<double, int>::iterator hueToAngleIterator = hueToAngleMap.begin();

public:
	ExperimentHandler() = default;
	ExperimentHandler(const ExperimentParameters& param);
	~ExperimentHandler() = default;

private:
	void printExperimentParameters() const;

public:
	void init();
	void step();
	void close();

private:
	bool bonafidePickAndPlace();
	void createShape();
	void graspShape();
	void placeShape();
	bool verifyDecision();
	void readShapeHue();
	void readTargetAngle();
	void getExpectedTargetAngle();

	bool mockPickAndPlace();
	void mockReadShapeHue();
	void mockReadTargetAngle();

	void degenerationProcedure();
	int getNumberOfElementsToDegenerate() const;

	void relearningProcedure();

	void cleanupPickAndPlace();
	void cleanupTrial();
	void saveLearningCyclesPerTrial() const;

	void backupWeightsFile() const;
	void restoreWeightsFile() const;
	bool doesBackupWeightsFileExist() const;
	void getOriginalWeightsFile() const;
};