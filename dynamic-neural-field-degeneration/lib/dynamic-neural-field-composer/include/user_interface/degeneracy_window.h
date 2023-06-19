#pragma once

#include "user_interface_window.h"

#include "./elements/field_coupling.h"
#include "./elements/neural_field.h"
#include "./elements/gauss_kernel.h"


class DegeneracyWindow : public UserInterfaceWindow
{
private:
	std::shared_ptr<Simulation> simulation;
	std::shared_ptr<FieldCoupling> coupling;
	std::shared_ptr<NeuralField> inputField;
	std::shared_ptr<NeuralField> outputField;
	std::shared_ptr<GaussKernel> gaussKernelInputField;
	std::shared_ptr<GaussKernel> gaussKernelOutputField;

	bool couplingSelected;
public:
	DegeneracyWindow(const std::shared_ptr<Simulation>& simulation);
	void render() override;
	~DegeneracyWindow() = default;
private:
	void renderCouplingSelector();
	void renderDegeneracyOptions();
	void renderFieldCentroids();
	void renderFieldCentroidsPlotOverSimulationIterations();
};