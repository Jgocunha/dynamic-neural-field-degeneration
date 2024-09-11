
#pragma once

#include <iostream>
#include <thread>
#include <iomanip>

#include "dnfcomposer_handler.h"
#include "experiment_parameters.h"

struct ExperimentData
{
	double inputFieldCentroid = -1;
	double outputFieldCentroid = -1;

	double targetInputFieldCentroid = -1; // Not used in the recovery experiment
	double targetOutputFieldCentroid = -1; // Not used in the recovery experiment
	std::vector<double> outputFieldCentroidHistory; // Not used in the recovery experiment
};

class ExperimentHandler
{
private:
	std::thread experimentThread;
	DnfcomposerHandler dnfcomposerHandler;
	// 	CoppeliasimHandler coppeliasimHandler;

	ExperimentParameters params;
	ExperimentData data;
	ExperimentStatistics stats; // In the recovery experiment
	// Signals signals; // In the recovery experiment

	std::unordered_map<double, int> hueToAngleMap;
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

	void readHueToAngleMap();

	//bool bonafidePickAndPlace();
	//void createShape();
	//void graspShape();
	//void placeShape();
	//bool verifyDecision();
	//void readShapeHue();
	//void readTargetAngle();
	//void getExpectedTargetAngle();
	//void initialDegeneration();

	//bool mockPickAndPlace();
	//void mockReadShapeHue();
	//void mockReadTargetAngle();

	//void degenerationProcedure();

	//void relearningProcedure();

	//void cleanupPickAndPlace();
	//void cleanupTrial();
	//void saveLearningCyclesPerTrial() const;

	//void backupWeightsFile() const;
	//void restoreWeightsFile() const;
	//bool doesBackupWeightsFileExist() const;
	//void getOriginalWeightsFile() const;

	//void createExperimentFolderDirectory();
	//void deleteExperimentFolderDirectory() const;

	//void saveWeights();
	//void readWeights();
};