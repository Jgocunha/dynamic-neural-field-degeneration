
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
		if(wasDegenerationRequested)
			activateDegeneration();
		if (wasRelearningRequested)
			activateRelearning();
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
	simulationParameters.degeneracyType = degeneracyType;
	wasDegenerationRequested = true;
}

void DnfcomposerHandler::setExternalInput(const double& position)
{
	this->simulationParameters.externalInputPosition = position;
	wasExternalInputUpdated = true;
}

void DnfcomposerHandler::setRelearning()
{
	wasRelearningRequested = true;
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

bool DnfcomposerHandler::getHasRelearningFinished()
{
	return hasRelearningFinished;
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

void DnfcomposerHandler::activateDegeneration()
{
	switch (simulationParameters.degeneracyType)
	{
	case ElementDegeneracyType::NEURONS_DEACTIVATE:
		simulationElements.inputField->setDegeneracyType(simulationParameters.degeneracyType);
		simulationElements.inputField->startDegeneration();
		break;
	case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
	case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
	case ElementDegeneracyType::WEIGHTS_REDUCE: // this is hardcoded to 0.4
		simulationElements.fieldCoupling->setDegeneracyType(simulationParameters.degeneracyType);
		simulationElements.fieldCoupling->startDegeneration();
		break;
	default:
		break;
	}
	application->step();
	wasDegenerationRequested = false;
}

void DnfcomposerHandler::activateRelearning()
{
	hasRelearningFinished = false;

	// set up the field coupling wizard
	FieldCouplingWizard fcpw{ simulation, "per - dec" };
	
	// add gaussian inputs
	double offset = 1.0;
	GaussStimulusParameters gsp = { 3, 15, 20 };
	
	std::vector<std::vector<double>> inputTargetPeaksForCoupling =
	{
		{ 00.00 + offset }, // red
		{ 40.60 + offset }, // orange
		{ 60.00 + offset }, // yellow
		{ 120.00 + offset }, // green
		{ 240.00 + offset }, // blue
		{ 274.15 + offset }, // indigo
		{ 281.79 + offset } // violet
	};
	std::vector<std::vector<double>> outputTargetPeaksForCoupling =
	{
		{ 22.50 + offset },
		{ 45.00 + offset },
		{ 67.50 + offset },
		{ 90.00 + offset },
		{ 112.5 + offset },
		{ 135.0 + offset },
		{ 157.5 + offset }
	};
	
	fcpw.setTargetPeakLocationsForNeuralFieldPre(inputTargetPeaksForCoupling);
	fcpw.setTargetPeakLocationsForNeuralFieldPost(outputTargetPeaksForCoupling);
	
	std::cout << "Finished setting up the field coupling wizard.\n";

	gsp.amplitude = 15;
	gsp.sigma = 3;
	
	fcpw.setGaussStimulusParameters(gsp);
	std::cout << "Finished setting up the gaussian stimulus parameters.\n";

	fcpw.simulateAssociation();
	std::cout << "Finished simulating association.\n";
	
	// only 1 iteration of training
	fcpw.trainWeights(1);
	std::cout << "Finished training weights.\n";

	wasRelearningRequested = false;
	hasRelearningFinished = true;
}

