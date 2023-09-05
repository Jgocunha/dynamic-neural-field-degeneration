#include "../include/user-interface-window.h"

ExperimentWindow::ExperimentWindow(const std::shared_ptr<Simulation>& simulation)
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

	if (ImGui::Begin("Plot of decision field centroid plot over time"))
		renderFieldCentroidOverTime();
	ImGui::End();
}

void ExperimentWindow::renderExperimentDetails() const
{
	ImGui::Text("Current trial is %d.", expWinParams.currentTrial);
	ImGui::Text("Current degeneration type is %s.", expWinParams.currentDegenerationType.c_str());
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

	ImGui::Text("Decision field centroid is %.2f", expWinParams.decisionFieldCentroid);
	ImGui::Text("Expected decision field centroid is %.2f", expWinParams.expectedDecisionFieldCentroid);
	ImGui::Text("Maximum allowed deviation is %.2f", expWinParams.maximumAllowedDeviation);

	// Check the condition and set the font color accordingly
	if (expWinParams.decisionFieldCentroidDeviation > expWinParams.maximumAllowedDeviation)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red color
		ImGui::Text("Decision field centroid is not within limits.");
	}
	else
	{
		// green color
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
		ImGui::Text("Decision field centroid is within limits.");
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

void ExperimentWindow::renderFieldCentroidOverTime() const
{
	static std::vector<double> decisionFieldCentroidValue;
	static int iteration = 0;


	if (expWinParams.decisionFieldCentroid > 0.0)
	{
		decisionFieldCentroidValue.push_back(expWinParams.decisionFieldCentroid);
	}
	else
	{
		decisionFieldCentroidValue.clear();
		iteration = 0;
	}

	ImPlotStyle& style = ImPlot::GetStyle();
	style.LineWeight = 3.0f;


	if (ImPlot::BeginPlot("Decision field centroid plot over time"))
	{
		ImPlot::SetupAxes("Simulation iterations", "Value of centroid");
		ImPlot::SetupLegend(ImPlotLocation_NorthEast, ImPlotLegendFlags_Outside);

		const std::string labelInputField = "Centroid of decision field";
		ImPlot::SetupAxisLimits(ImAxis_Y1, 35, 45, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_X1, 0, decisionFieldCentroidValue.size(), ImGuiCond_Always);
		ImPlot::PlotLine(labelInputField.c_str(), &decisionFieldCentroidValue[0], decisionFieldCentroidValue.size());

	}

	ImPlot::EndPlot();
	iteration++;
}

void ExperimentWindow::setCurrentTrial(const int& currentTrial)
{
	expWinParams.currentTrial = currentTrial;
}

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

double ExperimentWindow::calculateFieldCentroidDeviation(const double& fieldCentroid, const double& expectedFieldCentroid)
{
	return std::abs(fieldCentroid - expectedFieldCentroid);
}

void ExperimentWindow::setExperimentSetupData(const std::string& currentDegenerationType, const double& maximumAllowedDeviation, const std::string& typeOfElementsDegenerated)
{
	setCurrentDegenerationType(currentDegenerationType);
	setMaximumAllowedDeviation(maximumAllowedDeviation);
	setTypeOfElementsDegenerated(typeOfElementsDegenerated);
}

void ExperimentWindow::setCentroids(const double& perceptualFieldCentroid, const double& decisionFieldCentroid)
{
	setPerceptualFieldCentroid(perceptualFieldCentroid);
	setDecisionFieldCentroid(decisionFieldCentroid);

	setPerceptualFieldCentroidDeviation(calculateFieldCentroidDeviation(perceptualFieldCentroid, expWinParams.expectedPerceptualFieldCentroid)) ;
	setDecisionFieldCentroidDeviation(calculateFieldCentroidDeviation(decisionFieldCentroid, expWinParams.expectedDecisionFieldCentroid));
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

void ExperimentWindow::setExpectedPerceptualFieldCentroid(const double& expectedPerceptualFieldCentroid)
{
		expWinParams.expectedPerceptualFieldCentroid = expectedPerceptualFieldCentroid;
}

void ExperimentWindow::setExpectedDecisionFieldCentroid(const double& expectedDecisionFieldCentroid)
{
	expWinParams.expectedDecisionFieldCentroid = expectedDecisionFieldCentroid;
}

void ExperimentWindow::setPerceptualFieldCentroidDeviation(const double& perceptualFieldCentroidDeviation)
{
	expWinParams.perceptualFieldCentroidDeviation = perceptualFieldCentroidDeviation;
}

void ExperimentWindow::setDecisionFieldCentroidDeviation(const double& decisionFieldCentroidDeviation)
{
	expWinParams.decisionFieldCentroidDeviation = decisionFieldCentroidDeviation;
}

