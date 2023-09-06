
#pragma once

#include <iostream>
#include <thread>

#include "./dnfcomposer-handler.h"

struct ExperimentParameters
{
	std::string filePathPrefix = "../../../data/";

	int numberOfStimulusPerTrial = 7;
	double decisionTolerance = 5;
	int numberOfTrials = 10;

	ElementDegeneracyType degeneracyType = ElementDegeneracyType::WEIGHTS_DEACTIVATE;
	std::string degeneracyName = "deactivate-weights";
	std::string typeOfElementsDegenerated = "weights";
	std::string fieldToDegenerate = "perceptual";

	int initialPercentageOfDegeneration = 0;
	int targetPercentageOfDegeneration = 100;
	int currentPercentageOfDegeneration = 0;

	bool isDataSavingOn = false;
	bool isVisualizationOn = true;
	bool isDebugModeOn = true;

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
};

struct ExperimentData
{
	double inputFieldCentroid = 0.0;
	double outputFieldCentroid = 0.0;
	double targetInputFieldCentroid = 0.0;
	double targetOutputFieldCentroid = 0.0;
	std::vector<std::vector<double>> outputFieldCentroidHistory;
};

struct ExperimentStatistics
{
	enum class DecisionClassification
	{
		CORRECT = 0,
		INCORRECT = 1,
		NO_DECISION = 2
	};
	DecisionClassification decision = DecisionClassification::NO_DECISION;
	std::vector<DecisionClassification> decisionHistory;
};

class ExperimentHandler
{
private:
	DnfcomposerHandler dnfcomposerHandler;
	std::thread experimentThread;

	ExperimentParameters params;
	ExperimentData data;
	ExperimentStatistics stats;

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

	void addExternalStimulus();
	void readTargetAngle();
	void getExpectedTargetAngle();
	void verifyDecisionMaking();
	bool hasOutputFieldDegenerated() const;
	void degenerationProcedure();
	void cleanUpTrial();

	//bool hasOutputFieldDegenerated() const;
	//void saveOutputFieldCentroidToFile() const;
};