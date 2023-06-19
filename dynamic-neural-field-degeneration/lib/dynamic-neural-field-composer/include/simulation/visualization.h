#pragma once

#include <iostream>

#include "simulation.h"
#include "exceptions/exception.h"

class Visualization
{
private:
	std::shared_ptr<Simulation> sim;
	std::vector<std::pair<std::string, std::vector<double>*>> plottingLabelAndData;
public:
	Visualization(std::shared_ptr<Simulation> sim);

	void setSimulation(std::shared_ptr<Simulation> sim);
	void addPlottingData(const std::string& elementId, const std::string& componentId);

	std::shared_ptr<const Simulation> getAssociatedSimulationPtr() const;
	std::string getPlottingLabel(const int& index) const;
	std::vector<double>* getPlottingData(const int& index) const;
	uint8_t getNumberOfPlots() const;

	~Visualization() = default;
};