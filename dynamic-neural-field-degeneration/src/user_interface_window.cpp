#include "user_interface_window.h"



ExperimentWindow::ExperimentWindow(const std::shared_ptr<dnf_composer::Simulation>& simulation)
	: simulation(simulation)
{
}

void ExperimentWindow::render()
{
	if (ImGui::Begin("Plot of output field centroid plot over time"))
		renderFieldCentroidOverTime();
	ImGui::End();
}

void ExperimentWindow::renderFieldCentroidOverTime() const
{
	static std::vector<double> decisionFieldCentroidValue;
	static int iteration = 0;

	const auto decisionField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement("output field"));
	const auto centroid = decisionField->getCentroid();

	if (centroid > 0.0)
	{
		decisionFieldCentroidValue.push_back(centroid);
	}
	else
	{
		decisionFieldCentroidValue.clear();
		iteration = 0;
	}

	if (decisionFieldCentroidValue.empty())
	{
		return;
	}

	const auto maxCentroidValue = *std::max_element(decisionFieldCentroidValue.begin(), decisionFieldCentroidValue.end());
	const auto minCentroidValue = *std::min_element(decisionFieldCentroidValue.begin(), decisionFieldCentroidValue.end());

	ImPlotStyle& style = ImPlot::GetStyle();
	style.LineWeight = 3.0f;


	if (ImPlot::BeginPlot("Output field centroid plot over time"))
	{
		ImPlot::SetupAxes("Simulation iterations", "Value of centroid");
		ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Horizontal);

		const std::string labelInputField = "Centroid of output field";
		ImPlot::SetupAxisLimits(ImAxis_Y1, minCentroidValue - 0.5, maxCentroidValue + 0.5, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_X1, 0, decisionFieldCentroidValue.size(), ImGuiCond_Always);
		ImPlot::PlotLine(labelInputField.c_str(), &decisionFieldCentroidValue[0], decisionFieldCentroidValue.size());

	}

	ImPlot::EndPlot();
	iteration++;
}


