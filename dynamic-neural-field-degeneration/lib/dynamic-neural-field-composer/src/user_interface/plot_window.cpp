#include "./user_interface/plot_window.h"


PlotWindow::PlotWindow(const std::shared_ptr<Visualization>& visualization)
{
	this->visualization = visualization;
	numPlotWindows++;
	id = numPlotWindows;
}

void PlotWindow::render()
{
	//renderElementSelector();
	renderPlots();
}

void PlotWindow::renderPlots()
{
	configure();

	if (ImGui::Begin("Plot window"))
	{
		if (ImPlot::BeginPlot("Plot window"))
		{
			ImPlot::SetupAxes("Field position", "Amplitude");
			ImPlot::SetupLegend(ImPlotLocation_NorthEast, ImPlotLegendFlags_Outside);

			uint8_t numOfPlots = visualization->getNumberOfPlots();
			for (int j = 0; j < numOfPlots; j++)
			{
				std::string label = visualization->getPlottingLabel(j);
				std::vector<double> data = *visualization->getPlottingData(j);
				ImPlot::PlotLine(label.c_str(), &data[0], data.size());
			}

		}
		ImPlot::EndPlot();
	}
	ImGui::End();
}

void PlotWindow::renderElementSelector()
{
	auto simulation = visualization->getAssociatedSimulationPtr();
	const uint8_t numberOfElementsInSimulation = simulation->getNumberOfElements();

	static std::string selectedElementId{};
	static int currentElementIdx = 0;

	if (ImGui::Begin("Plot selector"))
	{
		if (ImGui::BeginListBox("Select element", { 200.0f, 100.0f }))
		{
			for (int n = 0; n < numberOfElementsInSimulation; n++)
			{
				auto element = simulation->getElement(n);
				std::string elementId = element->getUniqueIdentifier();
				const bool isSelected = (currentElementIdx == n);
				if (ImGui::Selectable(elementId.c_str(), isSelected))
				{
					selectedElementId = elementId;
					currentElementIdx = n;
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		const char* components[] = { "output", "activation", "input", "kernel" };
		static int currentComponentIdx = 0;

		if (ImGui::BeginListBox("Select component", { 200.0f, 100.0f }))
		{
			for (int n = 0; n < IM_ARRAYSIZE(components); n++)
			{
				const bool isSelected = (currentComponentIdx == n);
				if (ImGui::Selectable(components[n], isSelected))
					currentComponentIdx = n;

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		if (ImGui::Button("Add", { 100.0f, 30.0f }))
		{
			visualization->addPlottingData(selectedElementId, components[currentComponentIdx]);
		}
	}
	ImGui::End();
}

void PlotWindow::configure()
{
	ImPlot::SetNextAxesLimits(0, 100, -30, 40);
	ImPlotStyle& style = ImPlot::GetStyle();
	style.LineWeight = 3.0f;
}