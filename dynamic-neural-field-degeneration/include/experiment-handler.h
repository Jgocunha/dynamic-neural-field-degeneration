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

	int maximumAmountOfRelearningCycles = 10;

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
	int numOfRelearningCycles = 0;
	int shapesPlacedIncorrectly = 0;
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
};