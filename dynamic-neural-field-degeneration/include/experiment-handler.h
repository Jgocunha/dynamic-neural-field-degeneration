
#pragma once

#include <iostream>
#include <thread>

#include "./coppeliasim-handler.h"
#include "./dnfcomposer-handler.h"


struct ExperimentParameters
{
	int numberOfShapesPerTrial = 2;
	int decisionTolerance = 5;
	int numberOfTrials = 10;
	int percentageOfDegeneration = 65;
	ElementDegeneracyType degeneracyType = ElementDegeneracyType::WEIGHTS_DEACTIVATE;
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
		{40.6034, 40},
		{60.00,  65},
		{120.00, 90},
		{240.00, 115},
		{274.00, 140},
		{282.00, 165}
	};

public:
	ExperimentHandler(const ExperimentParameters& param);
	~ExperimentHandler() = default;

	void init();
	void step();
	void close();
private:
	void pickAndPlace();
	void pickAndPlaceWithLearning();

	void createShape();
	void graspShape();
	void placeShape();

	void readShapeHue();
	void readTargetAngle();

	void cleanUpTrial();
	void updateStatistics();

	bool verifyDecision();
	void relearningProcedure();
	void degenerationProcedure();
};