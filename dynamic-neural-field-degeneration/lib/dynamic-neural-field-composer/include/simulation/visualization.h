#pragma once

#include <iostream>

#include "simulation.h"
#include "exceptions/exception.h"

namespace dnf_composer
{
	class Visualization
	{
	private:
		std::shared_ptr<Simulation> simulation;
		std::vector<std::pair<std::string, std::vector<double>*>> plottingLabelAndData;
	public:
		Visualization(std::shared_ptr<Simulation> targetSimulation);

		Visualization(const Visualization&) = delete;
		Visualization& operator=(const Visualization&) = delete;
		Visualization(Visualization&&) = delete;
		Visualization& operator=(Visualization&&) = delete;

		void setSimulation(const std::shared_ptr<Simulation>& targetSimulation);
		void addPlottingData(const std::string& elementId, const std::string& componentId);

		std::shared_ptr<const Simulation> getAssociatedSimulationPtr() const;
		std::string getPlottingLabel(const int& index) const;
		std::vector<double>* getPlottingData(const int& index) const;
		int getNumberOfPlots() const;

		~Visualization() = default;
	};
}

