#pragma once


#include <memory>

#include "../lib/imgui/imgui.h"
#include "../lib/imgui/backends/imgui_impl_win32.h"
#include "../lib/imgui/backends/imgui_impl_dx12.h"

#include "../lib/implot/implot.h"
#include "../lib/implot/implot_internal.h"


#include "./simulation/visualization.h"

#define CHAR_SIZE 50

class UserInterfaceWindow
{
public:
	UserInterfaceWindow();
	virtual void render() = 0;
	~UserInterfaceWindow() = default;
};

