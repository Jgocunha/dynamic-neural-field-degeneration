#include "../include/user-interface-window.h"

ExperimentWindow::ExperimentWindow(const std::shared_ptr<dnf_composer::Simulation>& simulation)
	: simulation(simulation)
{
}

void ExperimentWindow::render()
{
	if (ImGui::Begin("Setup details"))
		renderExperimentDetails();
	ImGui::End();

	if (ImGui::Begin("Field analysis"))
		renderFieldAnalysis();
	ImGui::End();

	if (ImGui::Begin("Degeneration statistics"))
		renderDegenerationStatistics();
	ImGui::End();

	if (ImGui::Begin("Relearning statistics"))
		renderRelearningStatistics();
	ImGui::End();

}

// render functions

void ExperimentWindow::renderExperimentDetails() const
{
	ImGui::Text("Current trial is %d", expWinParams.currentTrial);
	ImGui::Text("Current degeneration type is %s", expWinParams.currentDegenerationType.c_str());
}

void ExperimentWindow::renderFieldAnalysis() const
{
	ImGui::Text("Perceptual field centroid is %.2f.", expWinParams.perceptualFieldCentroid);
	ImGui::Text("Expected perceptual field centroid is %.2f.", expWinParams.expectedPerceptualFieldCentroid);
	ImGui::Text("Maximum allowed deviation is %.2f.", expWinParams.maximumAllowedDeviation);

	// Check the condition and set the font color accordingly
	if (expWinParams.perceptualFieldCentroidDeviation > expWinParams.maximumAllowedDeviation)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red color
		ImGui::Text("Perceptual field centroid is not within limits.");
	}
	else
	{
		// green color
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
		ImGui::Text("Perceptual field centroid is within limits.");
	}

	ImGui::Text("Current deviation is %.2f.", expWinParams.perceptualFieldCentroidDeviation);

	// Reset the font color to the default
	ImGui::PopStyleColor();

	// Custom vertical spacing using a dummy element with a specific size
	ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Adjust the second parameter for the desired vertical spacing size

	ImGui::Text("Output field centroid is %.2f", expWinParams.decisionFieldCentroid);
	ImGui::Text("Expected output field centroid is %.2f", expWinParams.expectedDecisionFieldCentroid);
	ImGui::Text("Maximum allowed deviation is %.2f", expWinParams.maximumAllowedDeviation);

	// Check the condition and set the font color accordingly
	if (expWinParams.decisionFieldCentroidDeviation > expWinParams.maximumAllowedDeviation)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red color
		ImGui::Text("Output field centroid is not within limits.");
	}
	else
	{
		// green color
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
		ImGui::Text("Output field centroid is within limits.");
	}

	ImGui::Text("Current deviation is %.2f.", expWinParams.decisionFieldCentroidDeviation);

	// Reset the font color to the default
	ImGui::PopStyleColor();
}

void ExperimentWindow::renderDegenerationStatistics() const
{
	ImGui::Text("Number of degenerated elements is %d.", expWinParams.numberOfDegeneratedElements);
	ImGui::Text("Type of elements degenerated is %s.", expWinParams.typeOfElementsDegenerated.c_str());
}

void ExperimentWindow::renderRelearningStatistics() const
{
	ImGui::Text("Relearning type is %s.", expWinParams.relearningType ? "only degenerated cases" : "all cases");
	ImGui::Text("Relearning epochs is %d.", expWinParams.relearningEpochs);
	ImGui::Text("Learning rate is %.2f.", expWinParams.learningRate);
	ImGui::Text("Update all weights is %s.", expWinParams.updateAllWeights ? "true" : "false");

	// Custom vertical spacing using a dummy element with a specific size
	ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Adjust the second parameter for the desired vertical spacing size

	ImGui::Text("Current number of relearning cycles is %d.", expWinParams.numOfRelearningCycles);
}

// public set functions

void ExperimentWindow::setExperimentSetupData(const std::string& currentDegenerationType, const double& maximumAllowedDeviation, const std::string& typeOfElementsDegenerated)
{
	setCurrentDegenerationType(currentDegenerationType);
	setMaximumAllowedDeviation(maximumAllowedDeviation);
	setTypeOfElementsDegenerated(typeOfElementsDegenerated);
}

void ExperimentWindow::setCurrentTrial(const int& currentTrial)
{
	expWinParams.currentTrial = currentTrial;
}

void ExperimentWindow::setCentroids(const double& perceptualFieldCentroid, const double& decisionFieldCentroid)
{
	setPerceptualFieldCentroid(perceptualFieldCentroid);
	setDecisionFieldCentroid(decisionFieldCentroid);

	setPerceptualFieldCentroidDeviation();
	setDecisionFieldCentroidDeviation();
}

void ExperimentWindow::setExpectedCentroids(const double& expectedPerceptualFieldCentroid, const double& expectedDecisionFieldCentroid)
{
	setExpectedPerceptualFieldCentroid(expectedPerceptualFieldCentroid);
	setExpectedDecisionFieldCentroid(expectedDecisionFieldCentroid);
}

void ExperimentWindow::setNumberOfDegeneratedElements(const int& numberOfDegeneratedElements)
{
	expWinParams.numberOfDegeneratedElements = numberOfDegeneratedElements;
}

void ExperimentWindow::setRelearningParameters(const int& relearningType, const int& relearningEpochs, const double& learningRate, const int& maximumAmountOfDemonstrations, const bool updateAllWeights)
{
	expWinParams.relearningType = relearningType;
	expWinParams.relearningEpochs = relearningEpochs;
	expWinParams.learningRate = learningRate;
	expWinParams.updateAllWeights = updateAllWeights;
	expWinParams.maximumAmountOfDemonstrations = maximumAmountOfDemonstrations;
}

void ExperimentWindow::setRelearningCycles(const int& numOfRelearningCycles)
{
	expWinParams.numOfRelearningCycles = numOfRelearningCycles;
}

// auxiliary functions

double ExperimentWindow::calculateDeviation(const double& val1, const double& val2, const double& size)
{
	const double diff = std::fmod(val2 - val1 + size, size);
	return (diff <= size / 2.0) ? diff : size - diff;
}
// private set functions

void ExperimentWindow::setCurrentDegenerationType(const std::string& currentDegenerationType)
{
	expWinParams.currentDegenerationType = currentDegenerationType;
}

void ExperimentWindow::setMaximumAllowedDeviation(const double& maximumAllowedDeviation)
{
	expWinParams.maximumAllowedDeviation = maximumAllowedDeviation;
}

void ExperimentWindow::setTypeOfElementsDegenerated(const std::string& typeOfElementsDegenerated)
{
	expWinParams.typeOfElementsDegenerated = typeOfElementsDegenerated;
}

void ExperimentWindow::setPerceptualFieldCentroid(const double& perceptualFieldCentroid)
{
	expWinParams.perceptualFieldCentroid = perceptualFieldCentroid;
}

void ExperimentWindow::setDecisionFieldCentroid(const double& decisionFieldCentroid)
{
	expWinParams.decisionFieldCentroid = decisionFieldCentroid;
}

void ExperimentWindow::setExpectedPerceptualFieldCentroid(const double& expectedPerceptualFieldCentroid)
{
	expWinParams.expectedPerceptualFieldCentroid = expectedPerceptualFieldCentroid;
}

void ExperimentWindow::setExpectedDecisionFieldCentroid(const double& expectedDecisionFieldCentroid)
{
	expWinParams.expectedDecisionFieldCentroid = expectedDecisionFieldCentroid;
}

void ExperimentWindow::setPerceptualFieldCentroidDeviation()
{
	const double val1 = expWinParams.perceptualFieldCentroid;
	const double val2 = expWinParams.expectedPerceptualFieldCentroid;
	constexpr double size = 360.0;
	expWinParams.perceptualFieldCentroidDeviation = calculateDeviation(val1, val2, size);
}

void ExperimentWindow::setDecisionFieldCentroidDeviation()
{
	const double val1 = expWinParams.decisionFieldCentroid;
	const double val2 = expWinParams.expectedDecisionFieldCentroid;
	constexpr double size = 180.0;
	expWinParams.decisionFieldCentroidDeviation = calculateDeviation(val1, val2, size);
}