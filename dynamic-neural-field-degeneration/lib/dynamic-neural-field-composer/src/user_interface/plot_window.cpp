// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "./user_interface/plot_window.h"

namespace dnf_composer
{
	namespace user_interface
	{
		PlotWindow::PlotWindow(const std::shared_ptr<Simulation>& simulation)
			:simulation(simulation)
		{
			PlotParameters parameters;
			parameters.id = ++current_id;
			parameters.dimensions = { 0, 100, -30, 40 };
			parameters.annotations.title = "Plot window ";
			parameters.annotations.x_label = "Spatial dimension";
			parameters.annotations.y_label = "Amplitude";
			createPlot(parameters);
		}

		PlotWindow::PlotWindow(const std::shared_ptr<Simulation>& simulation, PlotParameters parameters)
			:simulation(simulation)
		{
			createPlot(parameters);
		}

		PlotWindow::PlotWindow(const std::shared_ptr<Visualization>& visualization, PlotParameters parameters)
		{
			parameters.visualization = visualization;
			parameters.id = ++current_id;
			parameters.annotations.title += " " + std::to_string(parameters.id);
			plots.emplace_back(parameters);

			const std::string message = "Added a new plot to the application with id: " + parameters.annotations.title;
			LoggerWindow::addLog(LogLevel::_INFO, message.c_str());
		}

		void PlotWindow::render()
		{
			//renderPlotControl();
			for(const auto& plot : plots)
			{
				//renderElementSelector(plot);
				renderPlot(plot);
			}
		}

		void PlotWindow::renderPlotControl()
		{
			if (ImGui::Begin("Plot control"))
			{
				if (ImGui::CollapsingHeader("Create new plot"))
				{
					static char title[CHAR_SIZE] = "This is a plot title";
					ImGui::InputTextWithHint("title", "enter text here", title, IM_ARRAYSIZE(title));

					static char x_label[CHAR_SIZE] = "x label";
					ImGui::InputTextWithHint("x_label", "enter text here", x_label, IM_ARRAYSIZE(x_label));

					static char y_label[CHAR_SIZE] = "y label";
					ImGui::InputTextWithHint("y_label", "enter text here", y_label, IM_ARRAYSIZE(y_label));

					static int x_min = 0;
					ImGui::InputInt("x_min", &x_min, 1.0, 10.0);

					static int x_max = 100;
					ImGui::InputInt("x_max", &x_max, 1.0, 10.0);

					static int y_min = -10;
					ImGui::InputInt("y_min", &y_min, 1.0, 10.0);

					static int y_max = 20;
					ImGui::InputInt("y_max", &y_max, 1.0, 10.0);


					if (ImGui::Button("Add", { 100.0f, 30.0f }))
					{
						PlotParameters parameters;
						parameters.annotations = {title, x_label, y_label};
						parameters.dimensions = {x_min, x_max, y_min, y_max};
						createPlot(parameters);
					}
				}
			}
			ImGui::End();
		}

		void PlotWindow::createPlot( PlotParameters& parameters)
		{
			parameters.visualization = std::make_shared<Visualization>(simulation);
			parameters.id = ++current_id;
			parameters.annotations.title += " " + std::to_string(parameters.id);
			plots.emplace_back(parameters);

			const std::string message = "Added a new plot to the application with id: " + parameters.annotations.title;
			LoggerWindow::addLog(LogLevel::_INFO, message.c_str());
		}

		void PlotWindow::renderPlot(const PlotParameters& parameters)
		{
			configure(parameters.dimensions);

			const std::string plotWindowTitle = parameters.annotations.title + " window";

			if (ImGui::Begin(plotWindowTitle.c_str()))
			{
				ImVec2 plotSize = ImGui::GetContentRegionAvail();  // Get available size in the ImGui window
				plotSize.x -= 5.0f; // Subtract some padding
				plotSize.y -= 5.0f; // Subtract some padding

				if (ImPlot::BeginPlot(parameters.annotations.title.c_str(), plotSize))
				{
					ImPlot::SetupAxes(parameters.annotations.x_label.c_str(), parameters.annotations.y_label.c_str());
					ImPlot::SetupLegend(ImPlotLocation_NorthEast, ImPlotLegendFlags_Outside);

					const int numOfPlots = parameters.visualization->getNumberOfPlots();
					for (int j = 0; j < numOfPlots; j++)
					{
						std::string label = parameters.visualization->getPlottingLabel(j);
						std::vector<double> data = *parameters.visualization->getPlottingData(j);
						ImPlot::PlotLine(label.c_str(), data.data(), static_cast<int>(data.size()));
					}

				}
				ImPlot::EndPlot();
			}
			ImGui::End();
		}

		void PlotWindow::renderElementSelector(const PlotParameters& parameters)
		{
			const auto simulation = parameters.visualization->getAssociatedSimulationPtr();
			const int numberOfElementsInSimulation = simulation->getNumberOfElements();

			static std::string selectedElementId{};
			static int currentElementIdx = 0;

			const std::string selectorTitle = parameters.annotations.title + " plot selector";

			if (ImGui::Begin(selectorTitle.c_str()))
			{
				//ImGui::Columns(2, nullptr, false); // Use 2 columns

				// First column: List box for selecting an element
				ImGui::Text("Select element");
				if (ImGui::BeginListBox("##Element list", { 300.00f, 200.0f }))
				{
					for (int n = 0; n < numberOfElementsInSimulation; n++)
					{
						const auto element = simulation->getElement(n);
						std::string elementId = element->getUniqueName();
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

				// Next column: List box for selecting a component
				//ImGui::NextColumn();
				const char* components[] = { "output", "activation", "input", "kernel" };
				static int currentComponentIdx = 0;

				ImGui::Text("Select component");
				if (ImGui::BeginListBox("##Component list", { 250.0f, 200.0f }))
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

				// Reset columns
				ImGui::Columns(1);

				if (ImGui::Button("Add", { 100.0f, 30.0f }))
				{
					parameters.visualization->addPlottingData(selectedElementId, components[currentComponentIdx]);
				}
			}
			ImGui::End();
		}

		void PlotWindow::configure(const PlotDimensions& dimensions)
		{
			constexpr static int safeMargin = 1;
			ImPlot::SetNextAxesLimits(dimensions.xMin - safeMargin, dimensions.xMax + safeMargin,
				dimensions.yMin - safeMargin, dimensions.yMax + safeMargin);
			ImPlotStyle& style = ImPlot::GetStyle();
			style.LineWeight = 3.0f;
		}
	}
}