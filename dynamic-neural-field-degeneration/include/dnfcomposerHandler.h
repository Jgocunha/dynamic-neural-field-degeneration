#pragma once


#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm> // For std::lower_bound

#include "../lib/dynamic-neural-field-composer/include/application/application.h"
#include "../lib/dynamic-neural-field-composer/include/elements/degenerate_neural_field.h"
#include "../lib/dynamic-neural-field-composer/include/elements/degenerate_field_coupling.h"
#include "./experimentInterface.h"


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
	std::shared_ptr<Visualization> visualizationPer, visualizationDec;
	std::shared_ptr<Simulation> simulation;
	std::shared_ptr<DegenerateNeuralField> inputField, outputField;
	std::shared_ptr<DegenerateFieldCoupling> fieldCoupling;
	std::shared_ptr<ExperimentWindow> window;
	
	int timeForFieldToSettle;
	std::vector<double> centroids;

	DecisionEvaluation decisionResults;

	std::unordered_map<double, int> hueToAngleMap = {
		{00.00,  15},
		{40.6034,  40},
		{60.00,  65},
		{120.00, 90},
		{240.00, 115},
		{274.146, 140},
		{284.791, 165}
	};

protected:
	double cuboidHue = 0.0;
	double targetRobotAngle = 0.0;

public:
	DNFComposerHandler();
	DNFComposerHandler(const std::shared_ptr<Simulation> simulation, bool isGUIVisible, int timeForFieldToSettle);
	~DNFComposerHandler();

	void init();
	void step();
	void close();

	bool getUserRequestClose();
	void setExternalStimulus(const double& cuboidHue);
	double getTargetPlaceAngle();
	void getPerceptualFieldCentroid();
	void applyDegeneration(ElementDegeneracyType elementDegeneracyType);
	void saveCentroids();
	void applyDegenerationAtTheBeginning(ElementDegeneracyType elementDegeneracyType);
private:
	void updateStatistics();
	void verifyOutput();
	bool verifyRobotAngle();
};