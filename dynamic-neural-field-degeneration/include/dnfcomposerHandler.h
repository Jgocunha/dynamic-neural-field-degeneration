#pragma once


#include <thread>
#include <mutex>
#include <condition_variable>

#include "../lib/dynamic-neural-field-composer/include/application/application.h"
#include "../lib/dynamic-neural-field-composer/include/elements/degenerate_neural_field.h"
#include "./experimentWindows.h"


struct DecisionEvaluation
{
	int numDecisions = 0;
	int numCorrectDecisions = 0;
	int numIncorrectDecisions = 0;
	double decisionRatio = 0.0f;

	void resetEvaluation()
	{
		numDecisions = 0;
		numCorrectDecisions = 0;
		numIncorrectDecisions = 0;
		decisionRatio = 0.0f;
	}
};

class DNFComposerHandler
{
private:
	std::shared_ptr<Application> application;
	std::shared_ptr<Visualization> visualizationPer;
	std::shared_ptr<Visualization> visualizationDec;
	std::shared_ptr<Simulation> simulation;
	std::shared_ptr<DegenerateNeuralField> inputField;
	std::shared_ptr<DegenerateNeuralField> outputField;
	std::shared_ptr<ExperimentWindow> window;
	bool userRequestClose = false;
	int timeForFieldToSettle = 50;

	DecisionEvaluation decisionResults;

protected:
	std::string cuboidColor;
	std::string targetBox;

	std::map<std::string, double> cuboidColorToCentroidMapping;
	std::map<std::string, double> targetBoxToCentroidMapping;

public:
	DNFComposerHandler() {}
	DNFComposerHandler(const std::shared_ptr<Simulation> simulation);
	~DNFComposerHandler();

	void init();
	void step();
	void close();

	void handleSignals();

	bool getUserRequestClose();
	void setExternalStimulus(const std::string& stimulusLabel);
	std::string getTargetBox();
private:
	void setupCuboidColorMap();
	void setupTargetBoxMap();
	void updateStatistics();
	void verifyOutput();
};