#include "../include/experimentWindows.h"

ExperimentWindow::ExperimentWindow(const std::shared_ptr<Simulation>& simulation)
	: simulation(simulation)
{
	currentTrial = 0;
	cuboidColor = "null";
	targetBox = "null";
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
	ImGui::Text("Cuboid color is %s", cuboidColor.c_str());
	ImGui::Text("Target box is %s", targetBox.c_str());
}

void ExperimentWindow::setCuboidColor(const std::string& cuboidColor)
{
	this->cuboidColor = cuboidColor;
}

void ExperimentWindow::setTargetBox(const std::string& targetBox)
{
	this->targetBox = targetBox;
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