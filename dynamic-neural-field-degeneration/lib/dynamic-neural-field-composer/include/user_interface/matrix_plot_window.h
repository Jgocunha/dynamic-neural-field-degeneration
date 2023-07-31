#pragma once


#include "user_interface_window.h"
#include "../elements/field_coupling.h"

class MatrixPlotWindow : public UserInterfaceWindow
{
private:
	std::shared_ptr<FieldCoupling> coupling;
public:
	MatrixPlotWindow(const std::shared_ptr<Simulation> simulation, const std::string& couplingUniqueId);
	void render() override;
	~MatrixPlotWindow() = default;
private:
	void displayMatrix();
	void displayHeatMap();
};
