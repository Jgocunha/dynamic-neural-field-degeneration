#pragma once

//#include "./dnfcomposerHandler.h"
#include "../lib/dynamic-neural-field-composer/include/user_interface/user_interface.h"

class ExperimentWindow : public UserInterfaceWindow //, public DNFComposerHandler
{
private:
	std::shared_ptr<Simulation> simulation;
	int currentTrial;
	std::string cuboidColor;
	std::string targetBox;
	double correctDecisionRatio;
	int numCorrectDecisions;
	std::string currentDegeneration;
public:
	ExperimentWindow(const std::shared_ptr<Simulation>& simulation);
	void render() override;
	~ExperimentWindow() = default;

	void setCuboidColor(const std::string& cuboidColor);
	void setTargetBox(const std::string& targetBox);
	
	void setCurrentTrial(const int& currentTrial);
	void setDecisionRatio(const double& decisionRatio);
	void setNumCorrectDecisions(const int& numCorrectDecisions);
private:
	void renderShapeDetails();
	void renderExperimentStatistics();
};