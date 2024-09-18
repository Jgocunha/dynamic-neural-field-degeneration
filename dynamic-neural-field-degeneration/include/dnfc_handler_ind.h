#pragma once

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

namespace experiment
{
	namespace degeneration
	{
		struct SimulationElements
		{
			std::shared_ptr<DegenerateNeuralField> inputField, outputField;
			std::shared_ptr<DegenerateFieldCoupling> fieldCoupling;
		};

		struct SimulationParameters
		{
			std::string inputFieldId = "perceptual field";
			std::string outputFieldId = "output field";
			std::string fieldCouplingId = "per - out";
			double externalInputPosition = 0;
			double inputFieldCentroid, outputFieldCentroid;
			int timeForFieldToSettle = 25;
			ElementDegeneracyType degeneracyType = ElementDegeneracyType::NONE;
			std::string fieldToDegenerate = "perceptual";

			bool isDebugMode = false;
			bool isUserInterfaceActive = false;
		};

		class DnfcomposerHandlerInducing
		{
		private:
			std::thread dnfcomposerThread;
			std::thread readCentroidsThread;

			std::unique_ptr<dnf_composer::Application> application;
			std::shared_ptr<dnf_composer::Simulation> simulation;
			std::shared_ptr<ExperimentWindow> userInterfaceWindow;

			SimulationElements simulationElements;
			SimulationParameters simulationParameters;

			int numberOfDegeneratedElements = 0;
			int numberOfElementsToDegenerate = 0;

			bool wasIntializationRequested = false;
			bool wasExternalInputUpdated = false;
			bool wasDegenerationRequested = false;
			bool haveFieldsSettled = false;
			bool hasTrialFinished = false;
			bool hasExperimentFinished = false;
		public:
			DnfcomposerHandlerInducing();
			DnfcomposerHandlerInducing(bool isUserInterfaceActive);

			~DnfcomposerHandlerInducing() = default;

			void init();
			void step();
			void close();
			void stop();

			void closeSimulation();

			void setDegeneracy(ElementDegeneracyType degeneracyType, const std::string& fieldToDegenerate);
			void setExternalInput(const double& position);
			void setHaveFieldsSettled(bool haveFieldsSettled);
			void setIsUserInterfaceActiveAs(bool isUserInterfaceActive) const;

			void setNumberOfElementsToDegenerate(int count);

			double getInputFieldCentroid() const;
			double getOutputFieldCentroid() const;
			bool getHaveFieldsSettled() const;
			std::shared_ptr<ExperimentWindow> getUserInterfaceWindow();

			void updateFieldCentroids();
			void initializeFields();
		private:
			void setupUserInterface();
			void updateExternalInput();
			void activateDegeneration();
			void waitForFieldsToSettle() const;

			void cleanUpTrial();
		};
	}
}
