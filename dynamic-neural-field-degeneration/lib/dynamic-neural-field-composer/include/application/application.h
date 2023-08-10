#pragma once

#include "exceptions/exception.h"
#include "simulation/simulation.h"
#include "simulation/visualization.h"
#include "user_interface/user_interface.h"

class Application
{
private:
	std::shared_ptr<Simulation> simulation;
	std::vector<std::shared_ptr<Visualization>> visualizations;
	std::shared_ptr<UserInterface> userInterface;
	bool activateUserInterface;
public:
	Application(std::shared_ptr<Simulation> simulation, 
		bool activateUserInterface = true);

	void init();
	void step();
	void close();

	void activateUserInterfaceWindow(const std::shared_ptr<UserInterfaceWindow> window);
	void setActivateUserInterfaceAs(bool activateUserInterface);

	const bool getCloseUI();
	~Application();
private:
	void addVisualization();
};