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
public:
	PlotWindow(const std::shared_ptr<Simulation>& simulation);
	PlotWindow(const std::shared_ptr<Simulation>& simulation, const PlotDimensions& dimensions);
	PlotWindow(const std::shared_ptr<Visualization>& visualization);
	PlotWindow(const std::shared_ptr<Visualization>& visualization, const PlotDimensions& dimensions);
	void render() override;
	~PlotWindow() = default;
private:
	void renderPlots();
	void renderElementSelector();
	void configure();
};

