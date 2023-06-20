#pragma once

#include "../lib/dynamic-neural-field-composer/include/application/application.h"
#include "../lib/dynamic-neural-field-composer/include/elements/degenerate_neural_field.h"

class DNFComposerHandler
{
private:
	std::shared_ptr<Application> application;
	std::vector<std::shared_ptr<Visualization>> visualizations;
	std::shared_ptr<Simulation> simulation;
	std::shared_ptr<DegenerateNeuralField> inputField;
	std::shared_ptr<DegenerateNeuralField> outputField;
	bool userRequestClose = false;
	std::map<std::string, double> cuboidColor;
	std::map<std::string, double> targetBox;
	std::string cuboidColorLabel;

	int timeForFieldToSettle = 50;
public:
	DNFComposerHandler(const std::shared_ptr<Simulation> simulation);
	~DNFComposerHandler();

	void init();
	void step();
	void close();

	bool getUserRequestClose();
	void setExternalStimulus(const std::string& stimulusLabel);
	std::string getTargetBox();
private:
	void setupCuboidColorMap();
	void setupTargetBoxMap();
};