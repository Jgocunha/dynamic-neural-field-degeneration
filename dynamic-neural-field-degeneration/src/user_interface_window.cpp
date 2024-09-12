#include "user_interface_window.h"

#include <elements/neural_field.h>

#include "degenerate_neural_field.h"



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
//<<<<<<< HEAD
//	static std::vector<double> decisionFieldCentroidValue; //= { 0.0 };
//	//static std::vector<double> perceptualFieldCentroidValue;// = { 0.0 };
//	static int iteration = 0;
//
//	ImVec2 plotSize = ImGui::GetContentRegionAvail();  // Get available size in the ImGui window
//	plotSize.x -= 5.0f; // Subtract some padding
//	plotSize.y -= 5.0f; // Subtract some padding
//
//	static constexpr ImPlotFlags flags = ImPlotFlags_Crosshairs | ImPlotFlags_Equal;
//
//
//	//const auto perceptualField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement("perceptual field"));
//	//const auto perceptualFieldCentroid = perceptualField->getCentroid();
//
//	//if (perceptualFieldCentroid > 0.0)
//	//{
//	//	perceptualFieldCentroidValue.push_back(perceptualFieldCentroid);
//	//}
//	//else
//	//{
//	//	//perceptualFieldCentroidValue = { 0.0 };
//	//	iteration = 0;
//	//}
//
//	//ImPlotStyle& style2 = ImPlot::GetStyle();
//	//style2.LineWeight = 3.0f;
//
//	//if(ImPlot::BeginPlot("Perceptual field centroid", plotSize, flags))
//	//{
//	//	ImPlot::SetupAxes("Simulation iterations", "Value of centroid", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
//	//	ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Horizontal);
//
//	//	const std::string labelOutputField = "Centroid of perceptual field";
//	//	if (perceptualFieldCentroidValue.size() > 1)
//	//	{
//	//		ImPlot::SetupAxisLimits(ImAxis_Y1, perceptualFieldCentroidValue[0] - 5, 0 + 5, ImGuiCond_Always);
//	//		ImPlot::SetupAxisLimits(ImAxis_X1, 0, perceptualFieldCentroidValue.size(), ImGuiCond_Always);
//	//		ImPlot::PlotLine(labelOutputField.c_str(), &perceptualFieldCentroidValue[0], perceptualFieldCentroidValue.size());
//	//	}
//	//}
//
//	//ImPlot::EndPlot();
//
//	const auto decisionField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement("output field"));
//	const auto decisionFieldCentroid = decisionField->getCentroid();
//
//	if (decisionFieldCentroid > 0.0)
//	{
//		decisionFieldCentroidValue.push_back(decisionFieldCentroid);
//	}
//	else
//	{
//		//decisionFieldCentroidValue = { 0.0 };
//		iteration = 0;
//	}
//
//	ImPlotStyle& style = ImPlot::GetStyle();
//	style.LineWeight = 3.0f;
//
//
//	if (ImPlot::BeginPlot("Output field centroid", plotSize, flags))
//	{
//		ImPlot::SetupAxes("Simulation iterations", "Value of centroid", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
//		ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Horizontal);
//
//		const std::string labelInputField = "Centroid of output field";
//
//		if (decisionFieldCentroidValue.size() > 1)
//		{
//			ImPlot::SetupAxisLimits(ImAxis_Y1, decisionFieldCentroidValue[0] - 5, decisionFieldCentroidValue[0] + 5, ImGuiCond_Always);
//			ImPlot::SetupAxisLimits(ImAxis_X1, 0, decisionFieldCentroidValue.size(), ImGuiCond_Always);
//			ImPlot::PlotLine(labelInputField.c_str(), &decisionFieldCentroidValue[0], decisionFieldCentroidValue.size());
//		}
//
//	}
//
//	ImPlot::EndPlot();
//
//
//=======
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


