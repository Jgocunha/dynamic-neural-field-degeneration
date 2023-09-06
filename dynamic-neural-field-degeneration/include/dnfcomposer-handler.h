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

	const int timeForFieldToSettle = 20;

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
	double expectedInputCentroid = 0.0, expectedOutputCentroid = 0.0;
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

public:
	DnfcomposerHandler();
	DnfcomposerHandler(bool isUserInterfaceActive);

	~DnfcomposerHandler() = default;
	
	void init();
	void step();
	void close();
	void stop();

	void closeSimulation();

	void setExperimentSetupData(const std::string& currentDegenerationType,
		const double& maximumAllowedDeviation, const std::string& typeOfElementsDegenerated) const;
	void setExpectedFieldBehavior(const double& targetPerceptualFieldCentroid, const double& targetDecisionFieldCentroid) const;
	void setTrial(const int& trial) const;
	void setRelearningParameters(const RelearningParameters::RelearningType& relearningType,
		const int& numberOfRelearningEpochs, const double& learningRate);


	void setDegeneracy(ElementDegeneracyType degeneracyType, const std::string& fieldToDegenerate);;
	void setExternalInput(const double& position);
	void setRelearning(const double& expectedInputCentroid, const double& expectedOutputCentroid);
	void setHaveFieldsSettled(bool haveFieldsSettled);
	void setHasRelearningFinished(bool hasRelearningFinished);
	void setIsUserInterfaceActiveAs(bool isUserInterfaceActive) const;

	double getInputFieldCentroid() const;
	double getOutputFieldCentroid() const;
	bool getHaveFieldsSettled() const;
	bool getHasRelearningFinished() const;
	std::shared_ptr<ExperimentWindow> getUserInterfaceWindow();

	void saveWeightsToFile() const;
private:
	void setupUserInterface();
	void updateExternalInput();
	void updateFieldCentroids();
	void activateDegeneration();
	void activateRelearning();
	void waitForFieldsToSettle() const;
};