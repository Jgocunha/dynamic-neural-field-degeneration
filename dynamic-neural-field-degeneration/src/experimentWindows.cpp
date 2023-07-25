#include "../include/experimentWindows.h"

ExperimentWindow::ExperimentWindow(const std::shared_ptr<Simulation>& simulation)
	: simulation(simulation)
{
	currentTrial = 0;
	cuboidHue = 0.0;
	robotTargetAngle = 0.0;
	correctDecisionRatio = 0;
	currentDegeneration = "null";
	numCorrectDecisions = 0;
}

void ExperimentWindow::render()
{
	if (ImGui::Begin("Input and output"))
	{
		renderShapeDetails();
	}
	ImGui::End();

	if (ImGui::Begin("Experiment statistics"))
	{
		renderExperimentStatistics();
	}
	ImGui::End();
}

void ExperimentWindow::renderExperimentStatistics()
{
	ImGui::Text("Current trial is %d", currentTrial);
	ImGui::Text("Number of correct decisions is %d", numCorrectDecisions);
	ImGui::Text("Correct decision percentage %.2f", correctDecisionRatio, "%");
}

void ExperimentWindow::renderShapeDetails()
{
	ImGui::Text("Cuboid hue is %.2f", cuboidHue);
	ImGui::Text("Target place angle is %.2f", robotTargetAngle);
}

void ExperimentWindow::setCuboidHue(const double& cuboidHue)
{
	this->cuboidHue = cuboidHue;
}

void ExperimentWindow::setTargetRobotAngle(const double& robotTargetAngle)
{
	this->robotTargetAngle = robotTargetAngle;
}

void ExperimentWindow::setCurrentTrial(const int& currentTrial)
{
	this->currentTrial = currentTrial;
}

void ExperimentWindow::setDecisionRatio(const double& decisionRatio)
{
	this->correctDecisionRatio = decisionRatio;
}

void ExperimentWindow::setNumCorrectDecisions(const int& numCorrectDecisions)
{
	this->numCorrectDecisions = numCorrectDecisions;
}