#pragma once


#include "user_interface_window.h"

static int current_id = 0;

struct PlotDimensions
{
	int xMin, xMax, yMin, yMax;
};


class PlotWindow : public UserInterfaceWindow
{
private:
	std::shared_ptr<Visualization> visualization;
	 int id;
	 PlotDimensions plotDimensions;
	 bool renderPlotSelector;
public:
	PlotWindow(const std::shared_ptr<Simulation>& simulation, bool renderPlotSelector = true);
	PlotWindow(const std::shared_ptr<Simulation>& simulation, const PlotDimensions& dimensions, bool renderPlotSelector = true);
	PlotWindow(const std::shared_ptr<Visualization>& visualization, bool renderPlotSelector = true);
	PlotWindow(const std::shared_ptr<Visualization>& visualization, const PlotDimensions& dimensions, bool renderPlotSelector = true);
	void render() override;
	~PlotWindow() = default;
private:
	void renderPlots();
	void renderElementSelector();
	void configure();
};

