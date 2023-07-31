#include "./user_interface/plot_window.h"


PlotWindow::PlotWindow(const std::shared_ptr<Simulation>& simulation, bool renderPlotSelector)
	:renderPlotSelector(renderPlotSelector)
{
	this->visualization = std::make_shared<Visualization>(simulation);
	id = ++current_id;
	plotDimensions = { 0, 100, -30, 40 };
}

PlotWindow::PlotWindow(const std::shared_ptr<Visualization>& visualization, bool renderPlotSelector)
	:renderPlotSelector(renderPlotSelector)
{
	this->visualization = visualization;
	id = ++current_id;
	plotDimensions = { 0, 100, -30, 40 };
}

PlotWindow::PlotWindow(const std::shared_ptr<Simulation>& simulation, const PlotDimensions& dimensions, bool renderPlotSelector)
	:plotDimensions(dimensions), renderPlotSelector(renderPlotSelector)
{
	this->visualization = std::make_shared<Visualization>(simulation);
	id = ++current_id;
}

PlotWindow::PlotWindow(const std::shared_ptr<Visualization>& visualization, const PlotDimensions& dimensions, bool renderPlotSelector)
	:plotDimensions(dimensions), renderPlotSelector(renderPlotSelector)
{
	this->visualization = visualization;
	id = ++current_id;
}

void PlotWindow::render()
{
	if(renderPlotSelector)
		renderElementSelector();
	renderPlots();
}

void PlotWindow::renderPlots()
{
	configure();

	std::string plotTitle = "Plot window " + std::to_string(id);
	if (ImGui::Begin(plotTitle.c_str()))
	{
		if (ImPlot::BeginPlot(plotTitle.c_str()))
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

	std::string selectorTitle = "Plot selector " + std::to_string(id);

	if (ImGui::Begin(selectorTitle.c_str()))
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
	ImPlot::SetNextAxesLimits(plotDimensions.xMin, plotDimensions.xMax, plotDimensions.yMin, plotDimensions.yMax);
	ImPlotStyle& style = ImPlot::GetStyle();
	style.LineWeight = 3.0f;
}