#include "simulation/visualization.h"


Visualization::Visualization(std::shared_ptr<Simulation> sim)
{
	if (sim == nullptr)
		throw Exception(ErrorCode::VIS_INVALID_SIM);

	this->sim = std::move(sim);
	plottingLabelAndData = {};
}

void Visualization::setSimulation(std::shared_ptr<Simulation> sim)
{
	if (sim == nullptr)
		throw Exception(ErrorCode::VIS_INVALID_SIM);

	this->sim = sim;
}

void Visualization::addPlottingData(const std::string& elementId, const std::string& componentId)
{
	std::vector<double>* data = sim->getComponentPtr(elementId, componentId);

	if (!data)
		throw Exception(ErrorCode::VIS_DATA_NOT_FOUND, elementId, componentId);

	for (const auto& [label, data] : plottingLabelAndData)
		if (label == elementId + " " + componentId)
		{
			std::cout << "Data already exists in the plotting vector.\n";
			return;
		}

	plottingLabelAndData.emplace_back(elementId + " " + componentId, data);
}

std::shared_ptr<const Simulation> Visualization::getAssociatedSimulationPtr() const
{
	return this->sim;
}

std::string Visualization::getPlottingLabel(const int& index) const 
{
	if (index < 0 || index >= plottingLabelAndData.size())
		throw Exception(ErrorCode::VIS_INVALID_PLOTTING_INDEX);

	return plottingLabelAndData[index].first;
}

std::vector<double>* Visualization::getPlottingData(const int& index) const
{
	if (index < 0 || index >= plottingLabelAndData.size())
		throw Exception(ErrorCode::VIS_INVALID_PLOTTING_INDEX);

	return plottingLabelAndData[index].second;
}

uint8_t Visualization::getNumberOfPlots() const 
{
	return plottingLabelAndData.size();
}