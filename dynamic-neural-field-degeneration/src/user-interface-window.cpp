#include "../include/user-interface-window.h"

ExperimentWindow::ExperimentWindow(const std::shared_ptr<Simulation>& simulation)
	: simulation(simulation)
{
}

void ExperimentWindow::render()
{
	if (ImGui::Begin("Field analysis"))
		renderShapeDetails();
	ImGui::End();

	if (ImGui::Begin("Experiment statistics"))
		renderExperimentStatistics();
	ImGui::End();
}

void ExperimentWindow::renderExperimentStatistics()
{
	ImGui::Text("Current trial is %d", expWinParams.currentTrial);
	ImGui::Text("Number of correct decisions is %d", expWinParams.numCorrectDecisions);
	ImGui::Text("Correct decision percentage %.2f", expWinParams.correctDecisionRatio);
}

void ExperimentWindow::renderShapeDetails()
{
	ImGui::Text("Cuboid hue is %.2f", expWinParams.cuboidHue);
	ImGui::Text("Perceptual field centroid is %.2f", expWinParams.perceptualFieldCentroid);

	ImGui::Text("Expected place angle is %.2f", expWinParams.expectedTargetAngle);
	ImGui::Text("Decision field centroid is %.2f", expWinParams.decisionFieldCentroid);
}

void ExperimentWindow::setCuboidHue(const double& cuboidHue)
{
	expWinParams.cuboidHue = cuboidHue;
}

void ExperimentWindow::setDecisionFieldCentroid(const double& decisionFieldCentroid)
{
	expWinParams.decisionFieldCentroid = decisionFieldCentroid;
}

void ExperimentWindow::setCurrentTrial(const int& currentTrial)
{
	expWinParams.currentTrial = currentTrial;
}

void ExperimentWindow::setDecisionRatio(const double& decisionRatio)
{
	expWinParams.correctDecisionRatio = decisionRatio;
}

void ExperimentWindow::setNumCorrectDecisions(const int& numCorrectDecisions)
{
	expWinParams.numCorrectDecisions = numCorrectDecisions;
}

void ExperimentWindow::setExpectedTargetAngle(const double& expectedTargetAngle)
{
	expWinParams.expectedTargetAngle = expectedTargetAngle;
}

void ExperimentWindow::setPerceptualFieldCentroid(const double& perceptualFieldCentroid)
{
	expWinParams.perceptualFieldCentroid = perceptualFieldCentroid;
}

void ExperimentWindow::setStatistics(const int& currentTrial, const double& decisionRatio, const int& numCorrectDecisions)
{
	setCurrentTrial(currentTrial);
	setDecisionRatio(decisionRatio);
	setNumCorrectDecisions(numCorrectDecisions);
}

void ExperimentWindow::setData(const double& cuboidHue, const double& expectedTargetAngle)
{
	setCuboidHue(cuboidHue);
	setExpectedTargetAngle(expectedTargetAngle);

}

void ExperimentWindow::setCentroids(const double& perceptualFieldCentroid, const double& decisionFieldCentroid)
{
	setPerceptualFieldCentroid(perceptualFieldCentroid);
	setDecisionFieldCentroid(decisionFieldCentroid);
}
