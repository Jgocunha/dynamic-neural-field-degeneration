#pragma once

#include <thread>
#include <iostream>

#include "../lib/dynamic-neural-field-composer/include/application/application.h"
#include "./dnf-architecture.h"
#include "./macros.h"
#include "./user-interface-window.h"


struct SimulationElements
{
	std::shared_ptr<DegenerateNeuralField> inputField, outputField;
	std::shared_ptr<DegenerateFieldCoupling> fieldCoupling;
	FieldCouplingWizard fcpw;
};

struct SimulationParameters
{
	std::string inputFieldId = "perceptual field";
	std::string outputFieldId = "decision field";
	std::string fieldCouplingId = "per - dec";

	double externalInputPosition = 0.0, expectedOutputCentroid = 0.0;
	double inputFieldCentroid = 0.0, outputFieldCentroid = 0.0;

	const int timeForFieldToSettle = 30;

	ElementDegeneracyType degeneracyType = ElementDegeneracyType::NONE;
	std::string fieldToDegenerate = "perceptual";

	bool isDebugMode = false;
	bool isUserInterfaceActive = false;
};

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

	std::unique_ptr<Application> application;
	std::shared_ptr<Simulation> simulation;
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

	bool wasStartSimulationRequested = false;
	bool wasCloseSimulationRequested = false;

	const double offset = 1.0;
	const std::vector<std::vector<double>> inputTargetPeaksForCoupling =
	{
		{ 00.00 + offset }, // red 0
		{ 41.00 + offset }, // orange 1
		{ 60.00 + offset }, // yellow 2
		{ 120.00 + offset }, // green 3
		{ 240.00 + offset }, // blue 4
		{ 274.00 + offset }, // indigo 5
		{ 300.00 + offset } // violet 6
	};
	const std::vector<std::vector<double>> outputTargetPeaksForCoupling =
	{
		{ 15.00 + offset },
		{ 40.00 + offset },
		{ 65.00 + offset },
		{ 90.00 + offset },
		{ 115.00 + offset },
		{ 140.00 + offset },
		{ 165.00 + offset }
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
	void setRelearningParameters(const RelearningParameters::RelearningType& relearningType,
		const int& numberOfRelearningEpochs, const double& learningRate, const int& maximumRelearningCycles, bool updateAllWeights);
	

	void setDegeneracy(ElementDegeneracyType degeneracyType, const std::string& fieldToDegenerate);;
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

	void saveWeightsToFile() const;

	void updateFieldCentroids();
private:
	void setupUserInterface();
	void updateExternalInput();
	void activateDegeneration();
	void activateRelearning();
	void waitForFieldsToSettle() const;

	void allCasesRelearning();
	void onlyDegeneratedCasesRelearning();
};