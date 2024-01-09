#pragma once


#include "user_interface_window.h"
#include "simulation/visualization.h"


namespace dnf_composer
{
	namespace user_interface
	{
		static int current_id = 0;

		struct PlotDimensions
		{
			int xMin = 0, xMax = 0, yMin = 0, yMax = 0;
		};

		struct PlotAnnotations
		{
			std::string title{}, x_label{}, y_label{};
		};

		struct PlotParameters
		{
			int id;
			std::shared_ptr<Visualization> visualization;
			PlotDimensions dimensions;
			PlotAnnotations annotations;
		};

		class PlotWindow : public UserInterfaceWindow
		{
		private:
			std::vector<PlotParameters> plots;
			double xStep = 1.0;
		public:
			PlotWindow(const std::shared_ptr<Simulation>& simulation);
			PlotWindow(const std::shared_ptr<Simulation>& simulation, PlotParameters parameters);
			PlotWindow(const std::shared_ptr<Visualization>& visualization);
			PlotWindow(const std::shared_ptr<Visualization>& visualization, PlotParameters parameters, double xStep);

			void render() override;
			~PlotWindow() override = default;
		private:
			void createPlot(PlotParameters& parameters);
			void renderPlotControl();
			void renderPlot(const PlotParameters& parameters) const;
			static void renderElementSelector(const PlotParameters& parameters);
			static void configure(const PlotDimensions& dimensions);
		};
	}
}


