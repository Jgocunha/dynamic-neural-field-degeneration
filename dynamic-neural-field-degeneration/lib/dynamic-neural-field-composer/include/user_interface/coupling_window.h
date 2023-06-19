#pragma once

#include "user_interface/user_interface_window.h"

#include "./elements/field_coupling.h"
#include "./elements/gauss_stimulus.h"

class CouplingWindow : public UserInterfaceWindow
{
private:
	std::shared_ptr<Simulation> simulation;
	std::shared_ptr<Visualization> visualization;
	std::shared_ptr<FieldCoupling> coupling;
	ImVec2 mouseCoordinates;
public:
	CouplingWindow(const std::shared_ptr<Simulation>& simulation);
	void render() override;
	~CouplingWindow() = default;
private:
	void renderCouplingSelector();
	void renderPlots();
	void renderOperations();
	void checkForMousePress();

	std::vector<double> normalizeFieldActivation(std::vector<double>& vec, const double& restingLevel);
};