#pragma once

#include "../lib/dynamic-neural-field-composer/include/user_interface/user_interface.h"


struct ExperimentWindowParameters
{
	int currentTrial = 0;
	double perceptualFieldCentroid = 0.0, decisionFieldCentroid = 0.0;
	double cuboidHue = 0.0, expectedTargetAngle = 0.0;
	double correctDecisionRatio = 0.0;
	int numCorrectDecisions = 0;
	std::string currentDegeneration = "null";
};


class ExperimentWindow : public UserInterfaceWindow 
{
private:
	std::shared_ptr<Simulation> simulation;
	ExperimentWindowParameters expWinParams;
public:
	ExperimentWindow(const std::shared_ptr<Simulation>& simulation);
	void render() override;
	~ExperimentWindow() = default;

	void setDecisionRatio(const double& decisionRatio);
	void setCurrentTrial(const int& currentTrial);
	
	void setCuboidHue(const double& cuboidHue);
	void setExpectedTargetAngle(const double& expectedTargetAngle);
	void setNumCorrectDecisions(const int& numCorrectDecisions);
	void setDecisionFieldCentroid(const double& robotTargetAngle);
	void setPerceptualFieldCentroid(const double& perceptualFieldCentroid);

private:
	void renderShapeDetails();
	void renderExperimentStatistics();
};