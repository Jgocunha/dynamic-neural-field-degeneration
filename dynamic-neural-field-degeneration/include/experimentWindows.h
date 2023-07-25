#pragma once

//#include "./dnfcomposerHandler.h"
#include "../lib/dynamic-neural-field-composer/include/user_interface/user_interface.h"

class ExperimentWindow : public UserInterfaceWindow //, public DNFComposerHandler
{
private:
	std::shared_ptr<Simulation> simulation;
	int currentTrial;
	double cuboidHue;
	double robotTargetAngle;
	double correctDecisionRatio;
	int numCorrectDecisions;
	std::string currentDegeneration;
public:
	ExperimentWindow(const std::shared_ptr<Simulation>& simulation);
	void render() override;
	~ExperimentWindow() = default;

	void setCuboidHue(const double& cuboidHue);
	void setTargetRobotAngle(const double& robotTargetAngle);
	
	void setCurrentTrial(const int& currentTrial);
	void setDecisionRatio(const double& decisionRatio);
	void setNumCorrectDecisions(const int& numCorrectDecisions);
private:
	void renderShapeDetails();
	void renderExperimentStatistics();
};