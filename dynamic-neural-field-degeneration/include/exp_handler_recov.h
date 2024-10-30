#pragma once

#include <bitset>

#include "coppeliasim_handler.h"
#include "dnfc_handler_recov.h"
#include "experiment_parameters.h"


namespace experiment
{
	namespace relearning
	{
		struct ExperimentData
		{
			double inputFieldCentroid = -1;
			double outputFieldCentroid = -1;
			double lastOutputFieldCentroid = -1;
			double shapeHue = -1;
			double expectedTargetAngle = -1;
			bool isFieldDead = false;
		};

		struct ExperimentStatistics
		{
			int numOfRelearningCycles = 0;
			int shapesPlacedIncorrectly = 0;
			std::vector<int> learningCyclesPerTrialHistory;
		};

		class ExperimentHandlerRelearning
		{
		private:
			std::thread experimentThread;
			CoppeliasimHandler coppeliasimHandler;
			DnfcomposerHandler dnfcomposerHandler;

			ExperimentParameters parameters;
			ExperimentData data;
			ExperimentStatistics statistics;
			Signals signals;

			double currentPercentageOfDegeneration = 0.0;
			int numberOfShapesPerTrial = 0;

			std::unordered_map<double, int> hueToAngleMap;
			std::unordered_map<double, int>::iterator hueToAngleIterator = hueToAngleMap.begin();
		public:
			ExperimentHandlerRelearning();
			~ExperimentHandlerRelearning() = default;

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
			void initialDegeneration();

			bool mockPickAndPlace();
			void mockReadShapeHue();
			void mockReadTargetAngle();

			void degenerationProcedure();

			void relearningProcedure();

			void cleanupPickAndPlace();
			void cleanupTrial();
			void saveLearningCyclesPerTrial() const;

			void backupWeightsFile() const;
			void restoreWeightsFile() const;
			bool doesBackupWeightsFileExist() const;
			void getOriginalWeightsFile() const;

			void createExperimentFolderDirectory();
			void deleteExperimentFolderDirectory() const;

			void saveWeights();
			void readWeights();
			void readHueToAngleMap();
		};
	}
}