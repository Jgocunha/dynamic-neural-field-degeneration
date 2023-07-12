
#include "application/application.h"

Application::Application(std::shared_ptr<Simulation> simulation, std::vector<std::shared_ptr<Visualization>> visualizations, bool activateUserInterface)
	:simulation(simulation), visualizations(visualizations), activateUserInterface(activateUserInterface)
{
	if(this->visualizations.empty())
		addVisualization();
	if(activateUserInterface)
		userInterface = std::make_shared<UserInterface>(this->simulation, this->visualizations);
}

void Application::addVisualization()
{
	visualizations.push_back(std::make_shared<Visualization>(simulation));
}

void Application::init()
{
	simulation->init();
	if (activateUserInterface)
		userInterface->init();
}

void Application::step()
{
	simulation->step();
	if (activateUserInterface)
		userInterface->step();
}

void Application::close()
{
	simulation->close();
	if (activateUserInterface)
		userInterface->close();
}

const bool Application::getCloseUI()
{
	if (activateUserInterface)
		return userInterface->getCloseUI();
	return false;
}

Application::~Application()
{
	// no cleanup necessary
}

void Application::addWindow(std::shared_ptr<UserInterfaceWindow> window)
{
	userInterface->addWindow(window);
}