
#include "../include/dnfcomposer-handler.h"


DnfcomposerHandler::DnfcomposerHandler()
{
	std::shared_ptr <Simulation> simulation = getExperimentSimulation();
	application = std::make_unique<Application>(simulation, true);

	application->activateUserInterfaceWindow(std::make_shared<SimulationWindow>(simulation));	
}

void DnfcomposerHandler::init()
{
	std::cout << "Dnfcomposer Handler: Thread will start.\n";
	dnfcomposerThread = std::thread(&DnfcomposerHandler::step, this);
}

void DnfcomposerHandler::step()
{
	application->init();
	
	bool userRequestClose = false;
	while (!userRequestClose)
	{
		application->step();
		userRequestClose = application->getCloseUI();
	}
	
	application->close();
}

void DnfcomposerHandler::close()
{
	// Wait for the thread to finish its execution
	dnfcomposerThread.join();

	std::cout << "Dnfcomposer Handler: Thread has finished its execution.\n";
}
