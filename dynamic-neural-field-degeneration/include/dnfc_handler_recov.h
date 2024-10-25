#pragma once

#include <thread>
#include <iostream>
#include <vector>

#include <thread>
#include <application/application.h>
#include <user_interface/element_window.h>
#include <user_interface/main_window.h>
#include <user_interface/heatmap_window.h>
#include <user_interface/simulation_window.h>
#include <user_interface/plot_window.h>

#include "degenerate_field_coupling.h"
#include "degenerate_neural_field.h"
#include "degeneration_parameters.h"
#include "dnf_architecture.h"
#include "user_interface_window.h"
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

			const int timeForFieldToSettle = 60;

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
			std::vector<std::vector<double>> inputTargetPeaksForCoupling;
			std::vector<std::vector<double>> outputTargetPeaksForCoupling;
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

			void setRelearningParameters(const RelearningType& relearningType,
				const int& numberOfRelearningEpochs, const double& learningRate, const int& maximumRelearningCycles, bool updateAllWeights);
			void setIncrementOfDegenerationPercentage(double percentage);


			void setInitialNumberOfElementsToDegenerate(int count) const;

			void setDegeneracy(degeneration::ElementDegeneracyType degeneracyType, const std::string& fieldToDegenerate);;
			void setExternalInput(const double& position);
			void setRelearning(const int& targetRelearningPositions);
			void setHaveFieldsSettled(bool haveFieldsSettled);
			void setHasRelearningFinished(bool hasRelearningFinished);
			void setIsUserInterfaceActiveAs(bool isUserInterfaceActive);

			void setWasStartSimulationRequested(bool wasStartSimulationRequested);
			void setWasCloseSimulationRequested(bool wasCloseSimulationRequested);

			double getInputFieldCentroid() const;
			double getOutputFieldCentroid() const;
			bool getHaveFieldsSettled() const;
			bool getHasRelearningFinished() const;
			std::shared_ptr<ExperimentWindow> getUserInterfaceWindow();
			int getNumberOfDegeneratedElements() const;

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
			void readPeaksForCoupling();
		};
	}
}
