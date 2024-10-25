#pragma once

#include <bitset>

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

		struct Signals
		{
			bool createShape = false;
			bool isShapeCreated = false;
		
			bool graspShape = false;
			bool isShapeGrasped = false;
		
			bool placeShape = false;
			bool isShapePlaced = false;
		
			double shapeHue = -1;
			double targetAngle = -1;
		};

		struct LogMsg
		{
			int trial = 0;
			int relearningAttempt = 0;
			int maxRelearningAttempts = 0;
			double initialDegenerationPercentage = 0.0;
			double currentPercentageOfDegeneration = 0.0;
			double maxPercentageOfDegeneration = 0.0;
			std::bitset<7> shapes;
		};

		class ExperimentHandlerRelearning
		{
		private:
			std::thread experimentThread;
			DnfcomposerHandler dnfcomposerHandler;

			ExperimentParameters parameters;
			ExperimentData data;
			ExperimentStatistics statistics;
			Signals signals;
			LogMsg msg;

			double currentPercentageOfDegeneration = 0.0;
			int numberOfShapesPerTrial = 7;

			std::unordered_map<double, int> hueToAngleMap;
			std::unordered_map<double, int>::iterator hueToAngleIterator = hueToAngleMap.begin();
		public:
			ExperimentHandlerRelearning();
			~ExperimentHandlerRelearning() = default;

			void init();
			void step();
			void close();

		private:
			bool verifyDecision();
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
