
#include "../include/dnfcomposer-handler.h"


DnfcomposerHandler::DnfcomposerHandler()
{
	simulation = getExperimentSimulation();
	application = std::make_unique<Application>(simulation, true);

	simulationElements.inputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.inputFieldId));
	simulationElements.outputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.outputFieldId));
	simulationElements.fieldCoupling = std::dynamic_pointer_cast<DegenerateFieldCoupling>(simulation->getElement(simulationParameters.fieldCouplingId));

	setupUserInterface();
}

void DnfcomposerHandler::init()
{
	if(DEBUG)
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
		if(wasExternalInputUpdated)
			updateExternalInput();
		updateFieldCentroids();
		userRequestClose = application->getCloseUI();
	}

	application->close();
}

void DnfcomposerHandler::close()
{
	// Wait for the thread to finish its execution
	dnfcomposerThread.join();
	if (DEBUG)
		std::cout << "Dnfcomposer Handler: Thread has finished its execution.\n";
}

void DnfcomposerHandler::setDegeneracy(ElementDegeneracyType degeneracyType)
{
	switch (degeneracyType)
	{
	case ElementDegeneracyType::NEURONS_DEACTIVATE:
		simulationElements.inputField->setDegeneracyType(degeneracyType);
		simulationElements.inputField->startDegeneration();
		break;
	case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
	case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
	case ElementDegeneracyType::WEIGHTS_REDUCE: // this is hardcoded to 0.4
		simulationElements.fieldCoupling->setDegeneracyType(degeneracyType);
		simulationElements.fieldCoupling->startDegeneration();
		break;
	default:
		break;
	}

	simulation->step();
}

void DnfcomposerHandler::setExternalInput(const double& position)
{
	this->simulationParameters.externalInputPosition = position;
	wasExternalInputUpdated = true;
}

double DnfcomposerHandler::getInputFieldCentroid()
{
	return simulationParameters.inputFieldCentroid;
}

double DnfcomposerHandler::getOutputFieldCentroid()
{
	return simulationParameters.outputFieldCentroid;
}

bool DnfcomposerHandler::getHaveFieldsSettled()
{
	return haveFieldsSettled;
}

std::shared_ptr<ExperimentWindow> DnfcomposerHandler::getUserInterfaceWindow()
{
	return userInterfaceWindow;
}

void DnfcomposerHandler::setupUserInterface()
{
	std::shared_ptr<Visualization> visualization = std::make_shared<Visualization>(simulation);
	visualization->addPlottingData("perceptual field", "activation");
	PlotDimensions pd;
	pd = { 0, 360, -25, 30 };
	application->activateUserInterfaceWindow(std::make_shared<PlotWindow>(visualization, pd, false));

	visualization = std::make_shared<Visualization>(simulation);
	visualization->addPlottingData("decision field", "activation");
	pd = { 0, 180, -15, 25 };
	application->activateUserInterfaceWindow(std::make_shared<PlotWindow>(visualization, pd, false));

	userInterfaceWindow = std::make_shared<ExperimentWindow>(simulation);
	application->activateUserInterfaceWindow(userInterfaceWindow);

	application->activateUserInterfaceWindow(std::make_shared<MatrixPlotWindow>(simulation, "per - dec"));
}

void DnfcomposerHandler::updateExternalInput()
{
	static double offset = 1.0;
	GaussStimulusParameters gsp = { 3, 25, 20 };
	gsp.position = simulationParameters.externalInputPosition + offset;
	std::shared_ptr<GaussStimulus> stimulus(new GaussStimulus("stimulus", simulationElements.inputField->getSize(), gsp));

	simulation->addElement(stimulus);
	simulationElements.inputField->addInput(stimulus);

	for (int i = 0; i < simulationParameters.timeForFieldToSettle; i++)
		application->step();

	simulation->removeElement("stimulus");

	for (int i = 0; i < simulationParameters.timeForFieldToSettle; i++)
		application->step();

	wasExternalInputUpdated = false;
	haveFieldsSettled = true;
}

void DnfcomposerHandler::updateFieldCentroids()
{
	simulationParameters.inputFieldCentroid = simulationElements.inputField->calculateCentroid();
	simulationParameters.outputFieldCentroid = simulationElements.outputField->calculateCentroid();

	userInterfaceWindow->setCentroids(simulationParameters.inputFieldCentroid, simulationParameters.outputFieldCentroid);
}