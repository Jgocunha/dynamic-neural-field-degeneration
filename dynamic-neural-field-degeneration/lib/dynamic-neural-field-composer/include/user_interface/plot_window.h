#pragma once


#include "user_interface_window.h"


class PlotWindow : public UserInterfaceWindow
{
private:
	std::shared_ptr<Visualization> visualization;
	static inline uint8_t numPlotWindows = 0;
	uint8_t id;
public:
	PlotWindow(const std::shared_ptr<Visualization>& visualization);
	void render() override;
	~PlotWindow() = default;
private:
	void renderPlots();
	void renderElementSelector();
	void configure();
};

