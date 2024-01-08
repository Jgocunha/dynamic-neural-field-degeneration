// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "simulation/visualization.h"


namespace dnf_composer
{
	Visualization::Visualization(std::shared_ptr<Simulation> targetSimulation)
	{
		if (targetSimulation == nullptr)
			throw Exception(ErrorCode::VIS_INVALID_SIM);

		simulation = std::move(targetSimulation);
		plottingLabelAndData = {};
	}

	void Visualization::setSimulation(const std::shared_ptr<Simulation>& targetSimulation)
	{
		if (targetSimulation == nullptr)
			throw Exception(ErrorCode::VIS_INVALID_SIM);

		simulation = targetSimulation;
	}

	void Visualization::addPlottingData(const std::string& elementId, const std::string& componentId)
	{
		if (elementId.empty())
		{
			const std::string message = "Tried to add an invalid element component '" + elementId + "' - '" + componentId + "' to the plot.\n";
			log(LogLevel::WARNING, message);
			return;
		}

		std::vector<double>* data = simulation->getComponentPtr(elementId, componentId);

		if (!data)
		{
			const std::string message = "Tried to add an invalid element component '" + elementId + "' - '" + componentId + "' to the plot.\n";
			log(LogLevel::WARNING, message);
			return;
		}

		for (const auto& [label, plotData] : plottingLabelAndData)
			if (label == elementId + " " + componentId)
			{
				log(LogLevel::ERROR_, "Data already exists in the plotting vector.\n");
				return;
			}

		plottingLabelAndData.emplace_back(elementId + " " + componentId, data);
		log(LogLevel::INFO, "Added element component '" + elementId + "' - '" + componentId + "' to the plot.\n");
	}

	std::shared_ptr<Simulation> Visualization::getAssociatedSimulationPtr() const
	{
		return simulation;
	}

	std::string Visualization::getPlottingLabel(const int& index) const 
	{
		if (index < 0 || index >= static_cast<int>(plottingLabelAndData.size()))
			throw Exception(ErrorCode::VIS_INVALID_PLOTTING_INDEX);

		return plottingLabelAndData[index].first;
	}

	std::vector<double>* Visualization::getPlottingData(const int& index) const
	{
		if (index < 0 || index >= static_cast<int>(plottingLabelAndData.size()))
			throw Exception(ErrorCode::VIS_INVALID_PLOTTING_INDEX);

		return plottingLabelAndData[index].second;
	}

	int Visualization::getNumberOfPlots() const 
	{
		return static_cast<int>(plottingLabelAndData.size());
	}
}

