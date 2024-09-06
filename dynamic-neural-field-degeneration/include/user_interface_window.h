#pragma once

#include "imgui-platform-kit/user_interface.h"
#include "simulation/simulation.h"


class ExperimentWindow : public imgui_kit::UserInterfaceWindow
{
private:
	std::shared_ptr<dnf_composer::Simulation> simulation;
public:
	ExperimentWindow(const std::shared_ptr<dnf_composer::Simulation>& simulation);
	void render() override;
private:
	void renderFieldCentroidOverTime() const;
};