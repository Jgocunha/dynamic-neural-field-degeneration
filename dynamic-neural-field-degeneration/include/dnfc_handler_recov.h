#pragma once

#include <thread>
#include <iostream>
#include <vector>

#include "dnfc_handler_ind.h"
#include "relearning_parameters.h"


namespace experiment
{
	namespace relearning
	{
		struct SimulationElements
		{
			std::shared_ptr<DegenerateNeuralField> inputField, outputField;
			std::shared_ptr<DegenerateFieldCoupling> fieldCoupling;
			dnf_composer::LearningWizard fcpw;
		};

		struct SimulationParameters
		{
			std::string inputFieldId = "perceptual field";
			std::string outputFieldId = "output field";
			std::string fieldCouplingId = "per - out";

			double externalInputPosition = 0.0, expectedOutputCentroid = 0.0;
			double inputFieldCentroid = 0.0, outputFieldCentroid = 0.0;

			const int timeForFieldToSettle = 35;

			degeneration::ElementDegeneracyType degeneracyType = degeneration::ElementDegeneracyType::NONE;
			std::string fieldToDegenerate = "perceptual";

			bool isDebugMode = false;
			bool isUserInterfaceActive = false;

			double incrementOfDegenerationInPercentage;
		};

		class DnfcomposerHandler
		{
		private:
			std::thread dnfcomposerThread;
			std::thread readCentroidsThread;

			std::unique_ptr<dnf_composer::Application> application;
			std::shared_ptr<dnf_composer::Simulation> simulation;
			std::shared_ptr<ExperimentWindow> userInterfaceWindow;

			SimulationElements simulationElements;
			SimulationParameters simulationParameters;
			RelearningParameters relearningParameters;

			int numberOfDegeneratedElements = 0;
			int numberOfRelearningCycles = 0;

			bool wasExternalInputUpdated = false;
			bool wasDegenerationRequested = false;
			bool wasRelearningRequested = false;
			bool haveFieldsSettled = false;
			bool hasRelearningFinished = false;
			bool hasExperimentFinished = false;
			bool wasUpdateWeightsRequested = false;

			bool wasStartSimulationRequested = false;
			bool wasCloseSimulationRequested = false;

			bool wasSaveWeightsRequested = false;

			const double offset = 0.0;
			const std::vector<std::vector<double>> inputTargetPeaksForCoupling =
			{
				//{ 00.00 + offset }, // red 0
				//{ 41.00 + offset }, // orange 1
				{ 60.00 + offset }, // yellow 2
				//{ 120.00 + offset }, // green 3
				//{ 240.00 + offset }, // blue 4
				//{ 274.00 + offset }, // indigo 5
				//{ 300.00 + offset } // violet 6
			};
			const std::vector<std::vector<double>> outputTargetPeaksForCoupling =
			{
				//{ 2.00 + offset },
				//{ 6.00 + offset },
				{ 10.00 + offset },
				//{ 14.00 + offset },
				//{ 18.00 + offset },
				//{ 22.00 + offset },
				//{ 26.00 + offset }
			};


		public:
			DnfcomposerHandler();
			DnfcomposerHandler(bool isUserInterfaceActive);

			~DnfcomposerHandler() = default;

			void init();
			void step();
			void close();
			void stop();

			void startSimulation();
			void closeSimulation();

			void setExperimentSetupData(const std::string& currentDegenerationType,
				const double& maximumAllowedDeviation, const std::string& typeOfElementsDegenerated) const;
			void setExpectedFieldBehavior(const double& targetPerceptualFieldCentroid, const double& targetDecisionFieldCentroid) const;
			void setTrial(const int& trial) const;
			void setRelearningCycles(const int& relearningCycles) const;
			void setRelearningParameters(const RelearningType& relearningType,
				const int& numberOfRelearningEpochs, const double& learningRate, const int& maximumRelearningCycles, bool updateAllWeights);
			void setIncrementOfDegenerationPercentage(double percentage);


			void setInitialNumberOfElementsToDegenerate(int count) const;

			void setDegeneracy(degeneration::ElementDegeneracyType degeneracyType, const std::string& fieldToDegenerate);;
			void setExternalInput(const double& position);
			void setRelearning(const int& targetRelearningPositions);
			void setHaveFieldsSettled(bool haveFieldsSettled);
			void setHasRelearningFinished(bool hasRelearningFinished);
			void setIsUserInterfaceActiveAs(bool isUserInterfaceActive) const;

			void setWasStartSimulationRequested(bool wasStartSimulationRequested);
			void setWasCloseSimulationRequested(bool wasCloseSimulationRequested);

			double getInputFieldCentroid() const;
			double getOutputFieldCentroid() const;
			bool getHaveFieldsSettled() const;
			bool getHasRelearningFinished() const;
			std::shared_ptr<ExperimentWindow> getUserInterfaceWindow();
			int getNumberOfDegeneratedElements();

			void setDataFilePath(const std::string& filePath);

			void updateFieldCentroids();
			void updateWeights();
			void readWeights();
			void setNumberOfElementsToDegenerate() const;
			void saveWeights();


		private:
			void setupUserInterface();
			void updateExternalInput();
			void activateDegeneration();
			void activateRelearning();
			void waitForFieldsToSettle() const;

			void allCasesRelearning();
			void onlyDegeneratedCasesRelearning();
			void saveWeightsToFile();

		};
	}
}
