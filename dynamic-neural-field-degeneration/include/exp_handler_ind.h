#pragma once

#include <thread>
#include "experiment_parameters.h"
#include "dnfc_handler_ind.h"

namespace experiment
{
	namespace degeneration
	{
		struct ExperimentData
		{
			double inputFieldCentroid = -1;
			double outputFieldCentroid = -1;
			double targetInputFieldCentroid = -1;
			double targetOutputFieldCentroid = -1;
			std::vector<double> outputFieldCentroidHistory;
		};

		class ExperimentHandlerInducing
		{
		private:
			ExperimentParameters params;
			ExperimentData data;

			DnfcomposerHandlerInducing dnfcomposerHandler;
			std::thread experimentThread;


			std::unordered_map<double, int> hueToAngleMap;
			std::unordered_map<double, int>::iterator hueToAngleIterator = hueToAngleMap.begin();

		public:
			ExperimentHandlerInducing();
			~ExperimentHandlerInducing() = default;

			void init();
			void step();
			void close();

		private:
			void setExpectedFieldBehaviour();
			void setExperimentAsEnded();
			void setExperimentSetupData();

			void setupProcedure();
			void degenerationProcedure();
			void cleanUpTrial();

			bool hasOutputFieldDegenerated() const;
			void saveOutputFieldCentroidToFile() const;

			void readHueToAngleMap();
		};
	}
}