
#include "application/application.h"

Application::Application(std::shared_ptr<Simulation> simulation, bool activateUserInterface)
	:simulation(simulation), activateUserInterface(activateUserInterface)
{
	//if(this->visualizations.empty())
		//addVisualization();
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

void Application::setActivateUserInterfaceAs(bool activateUserInterface)
{
	//if (!activateUserInterface)
		//std::cout << "Deactivating User Interface. It will still be visible, but will not be interactable and will not reflect updated elements." << std::endl;
	//else
		//std::cout << "Activating User Interface." << std::endl;

	this->activateUserInterface = activateUserInterface;
}

void Application::activateUserInterfaceWindow(const std::shared_ptr<UserInterfaceWindow> window)
{
	if (activateUserInterface)
		userInterface->activateWindow(window);
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

