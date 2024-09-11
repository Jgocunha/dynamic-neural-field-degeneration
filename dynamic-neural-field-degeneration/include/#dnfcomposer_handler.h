#pragma once

#include <thread>
#include <iostream>
#include <elements/field_coupling.h>
#include <elements/neural_field.h>

#include "application/application.h"
#include "dnf_architecture.h"
#include "user_interface_window.h"
#include "degenerate_field_coupling.h"
#include "imgui-platform-kit/log_window.h"
#include "user_interface/plot_window.h"
#include "user_interface/simulation_window.h"
#include "user_interface/main_window.h"
#include "dnf_architecture.h"
#include "application/application.h"
#include <degenerate_neural_field.h>
#include "user_interface/element_window.h"
#include "user_interface/field_metrics_window.h"
#include "user_interface/heatmap_window.h"


struct SimulationElements
{
	std::shared_ptr<DegenerateNeuralField> inputField, outputField;
	std::shared_ptr<DegenerateFieldCoupling> fieldCoupling;
	dnf_composer::LearningWizard fcpw; // In recovery experiment
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
	double incrementOfDegenerationInPercentage; //In recovery experiment
};

// In recovery experiment
struct RelearningParameters
{
	enum class RelearningType
	{
		ALL_CASES = 0,
		ONLY_DEGENERATED_CASES = 1,
	};
	RelearningType relearningType = RelearningType::ALL_CASES;
	int numberOfRelearningEpochs = 0;
	double learningRate = 0.0;
	int targetRelearningPositions = 0;
	bool updateAllWeights = true;
};


class DnfcomposerHandler
{
private:
	std::thread dnfcomposerThread;
	std::thread readCentroidsThread;
	std::thread applicationThread; // This is not in the recovery experiment

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
	bool wasRelearningRequested = false; // This is in the recovery experiment
	bool haveFieldsSettled = false;
	bool hasRelearningFinished = false;
	bool hasExperimentFinished = false;
	bool wasUpdateWeightsRequested = false;

	bool wasIntializationRequested = false; // This is not in the recovery experiment
	bool hasTrialFinished = false; // This is not in the recovery experiment
	bool isCentroidDataBeingAccessed = false; // This is not in the recovery experiment

	bool wasStartSimulationRequested = false;
	bool wasCloseSimulationRequested = false;

	bool wasSaveWeightsRequested = false;

public:
	DnfcomposerHandler();
	DnfcomposerHandler(bool isUserInterfaceActive);

	~DnfcomposerHandler() = default;
	
	void init();
	void step();
	void close();
	void stop();

	void startSimulation(); // New function that is in the recovery experiment
	void closeSimulation();

	void setDegeneracy(ElementDegeneracyType degeneracyType, const std::string& fieldToDegenerate);
	void setExternalInput(const double& position);
	void setRelearning(const int& targetRelearningPositions); // New function that is in the recovery experiment
	void setHaveFieldsSettled(bool haveFieldsSettled);
	void setHasRelearningFinished(bool hasRelearningFinished); // New function that is in the recovery experiment
	void setIsUserInterfaceActiveAs(bool isUserInterfaceActive) const;

	void setCentroidDataBeingAccessed(bool isCentroidDataBeingAccessed);
	void setNumberOfElementsToDegenerate(const int& numberOfElementsToDegenerate);

	void setWasStartSimulationRequested(bool wasStartSimulationRequested);// New function that is in the recovery experiment
	void setWasCloseSimulationRequested(bool wasCloseSimulationRequested); // New function that is in the recovery experiment

	double getInputFieldCentroid() const;
	double getOutputFieldCentroid() const;
	bool getHaveFieldsSettled() const;
	bool getHasRelearningFinished() const; // New function that is in the recovery experiment
	std::shared_ptr<ExperimentWindow> getUserInterfaceWindow();
	int getNumberOfDegeneratedElements() const; // New function that is in the recovery experiment

	void setDataFilePath(const std::string& dataFilePath); // New function that is in the recovery experiment

	void updateFieldCentroids();
	void updateWeights(); // New function that is in the recovery experiment
	void readWeights(); // New function that is in the recovery experiment
	void setNumberOfElementsToDegenerate(); // Two versions of this function, one for the recovery experiment and one for the original experiment
	void saveWeights(); // New function that is in the recovery experiment

private:
	void setupUserInterface();
	void updateExternalInput();
	void activateDegeneration();
	void activateRelearning(); // New function that is in the recovery experiment
	void waitForFieldsToSettle() const;

	void initializeFields(); // This is not in the recovery experiment
	void cleanUpTrial(); // This is not in the recovery experiment

	void allCasesRelearning();
	void onlyDegeneratedCasesRelearning();
	void saveWeightsToFile();
};