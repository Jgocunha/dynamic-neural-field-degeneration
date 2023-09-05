
#include "../include/dnfcomposer-handler.h"


DnfcomposerHandler::DnfcomposerHandler()
{
	simulation = getExperimentSimulation();
	application = std::make_unique<Application>(simulation, true);

	simulationElements.inputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.inputFieldId));
	simulationElements.outputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.outputFieldId));
	simulationElements.fieldCoupling = std::dynamic_pointer_cast<DegenerateFieldCoupling>(simulation->getElement(simulationParameters.fieldCouplingId));
	
	simulationElements.fcpw = FieldCouplingWizard{simulation, "per - dec" };

	setupUserInterface();
}

DnfcomposerHandler::DnfcomposerHandler(bool isUserInterfaceActive)
{
	simulationParameters.isUserInterfaceActive = isUserInterfaceActive;

	simulation = getExperimentSimulation();
	application = std::make_unique<Application>(simulation, simulationParameters.isUserInterfaceActive);

	simulationElements.inputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.inputFieldId));
	simulationElements.outputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.outputFieldId));
	simulationElements.fieldCoupling = std::dynamic_pointer_cast<DegenerateFieldCoupling>(simulation->getElement(simulationParameters.fieldCouplingId));

	simulationElements.fcpw = FieldCouplingWizard{ simulation, "per - dec" };

	if (simulationParameters.isUserInterfaceActive)
		setupUserInterface();
}

// init step close and stop methods

void DnfcomposerHandler::init()
{
	dnfcomposerThread = std::thread(&DnfcomposerHandler::step, this);
	readCentroidsThread = std::thread(&DnfcomposerHandler::updateFieldCentroids, this);
}

void DnfcomposerHandler::step()
{
	application->init();
	
	bool userRequestClose = false;
	while (!userRequestClose && !hasExperimentFinished)
	{
		if (wasDegenerationRequested)
			activateDegeneration();
		else if (wasExternalInputUpdated)
			updateExternalInput();
		else if (wasRelearningRequested)
			activateRelearning();
		else
			application->step();

		userRequestClose = application->getCloseUI();
	}

	application->close();
}

void DnfcomposerHandler::close()
{
	// Wait for the thread to finish its execution
	dnfcomposerThread.join();
	readCentroidsThread.join();
}

void DnfcomposerHandler::stop()
{
	hasExperimentFinished = true;
}

// other methods

void DnfcomposerHandler::closeSimulation()
{
	numberOfDegeneratedElements = 0;
	numberOfRelearningCycles = 0;
	//simulation->close();
}

// UI setup

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

	//application->activateUserInterfaceWindow(std::make_shared<MatrixPlotWindow>(simulation, "per - dec"));
}

// public set methods for UI

void DnfcomposerHandler::setExperimentSetupData(const std::string& currentDegenerationType,
	const double& maximumAllowedDeviation, const std::string& typeOfElementsDegenerated) const
{
	if (simulationParameters.isUserInterfaceActive)
		userInterfaceWindow->setExperimentSetupData(currentDegenerationType, maximumAllowedDeviation, typeOfElementsDegenerated);
}

void DnfcomposerHandler::setExpectedFieldBehavior(const double& targetPerceptualFieldCentroid, const double& targetDecisionFieldCentroid) const
{
	if (simulationParameters.isUserInterfaceActive)
		userInterfaceWindow->setExpectedCentroids(targetPerceptualFieldCentroid, targetDecisionFieldCentroid);
}

void DnfcomposerHandler::setTrial(const int& trial) const
{
	if (simulationParameters.isUserInterfaceActive)
		userInterfaceWindow->setCurrentTrial(trial);
}

// public set methods for control flags

void DnfcomposerHandler::setDegeneracy(ElementDegeneracyType degeneracyType, const std::string& fieldToDegenerate)
{
	simulationParameters.degeneracyType = degeneracyType;
	simulationParameters.fieldToDegenerate = fieldToDegenerate;
	wasDegenerationRequested = true;
}

void DnfcomposerHandler::setExternalInput(const double& position)
{
	this->simulationParameters.externalInputPosition = position;
	wasExternalInputUpdated = true;
}

void DnfcomposerHandler::setRelearning(const double& expectedInputCentroid, const double& expectedOutputCentroid)
{
	relearningParameters.expectedInputCentroid = expectedInputCentroid;
	relearningParameters.expectedOutputCentroid = expectedOutputCentroid;
	wasRelearningRequested = true;
}

void DnfcomposerHandler::setHaveFieldsSettled(bool haveFieldsSettled)
{
	this->haveFieldsSettled = haveFieldsSettled;
}

void DnfcomposerHandler::setIsUserInterfaceActiveAs(bool isUserInterfaceActive) const
{
	application->setActivateUserInterfaceAs(isUserInterfaceActive);
}

// public get methods

double DnfcomposerHandler::getInputFieldCentroid() const
{
	return simulationParameters.inputFieldCentroid;
}

double DnfcomposerHandler::getOutputFieldCentroid() const
{
	return simulationParameters.outputFieldCentroid;
}

bool DnfcomposerHandler::getHaveFieldsSettled() const
{
	return haveFieldsSettled;
}

bool DnfcomposerHandler::getHasRelearningFinished() const
{
	return hasRelearningFinished;
}

std::shared_ptr<ExperimentWindow> DnfcomposerHandler::getUserInterfaceWindow()
{
	return userInterfaceWindow;
}

// Degeneration

void DnfcomposerHandler::activateDegeneration()
{
	static int elementCount = 0;
	elementCount++;
	switch (simulationParameters.degeneracyType)
	{
	case ElementDegeneracyType::NEURONS_DEACTIVATE:
		simulationElements.inputField->setDegeneracyType(simulationParameters.degeneracyType);
		simulationElements.inputField->startDegeneration();
		std::cout << "Deactivated " << elementCount << " neurons." << std::endl;
		break;
	case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
	case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
	case ElementDegeneracyType::WEIGHTS_REDUCE: // this is hardcoded to 0.4
		simulationElements.fieldCoupling->setDegeneracyType(simulationParameters.degeneracyType);
		simulationElements.fieldCoupling->startDegeneration();
		std::cout << "Deactivated " << elementCount << " weights." << std::endl;
		break;
	default:
		break;
	}
	//Sleep(2);
	//application->step();
	//Sleep(2);
	wasDegenerationRequested = false;
}

// Relearning

//void DnfcomposerHandler::activateRelearning()
//{
//	
//	// add gaussian inputs
//	double offset = 1.0;
//	GaussStimulusParameters gsp = { 3, 25, 20 };
//
//	std::vector<std::vector<double>> inputTargetPeaksForCoupling =
//	{
//		{relearningParameters.expectedInputCentroid + offset}
//	};
//
//	std::vector<std::vector<double>> outputTargetPeaksForCoupling =
//	{
//		{relearningParameters.expectedOutputCentroid + offset}
//	};
//	
//	simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPre(inputTargetPeaksForCoupling);
//	simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPost(outputTargetPeaksForCoupling);
//	
//	std::cout << "Finished setting up the field coupling wizard.\n";
//
//	gsp.amplitude = 15;
//	gsp.sigma = 3;
//	
//	simulationElements.fcpw.setGaussStimulusParameters(gsp);
//	std::cout << "Finished setting up the gaussian stimulus parameters.\n";
//
//	simulationElements.fcpw.simulateAssociation();
//	std::cout << "Finished simulating association.\n";
//	
//	// only 1 iteration of training
//	simulationElements.fcpw.trainWeights(1);
//	std::cout << "Finished training weights.\n";
//	
//	wasRelearningRequested = false;
//	hasRelearningFinished = true;
//}

void DnfcomposerHandler::activateRelearning()
{
	clearRelearning();

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
		{ 15.00 + offset },
		{ 40.00 + offset },
		{ 65.00 + offset },
		{ 90.00 + offset },
		{ 115.00 + offset },
		{ 140.00 + offset },
		{ 165.00 + offset }
	};

	simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPre(inputTargetPeaksForCoupling);
	simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPost(outputTargetPeaksForCoupling);
		
	//std::cout << "Finished setting up the field coupling wizard.\n";
	
	gsp.amplitude = 15;
	gsp.sigma = 3;
		
	simulationElements.fcpw.setGaussStimulusParameters(gsp);
	//std::cout << "Finished setting up the gaussian stimulus parameters.\n";
	
	simulationElements.fcpw.simulateAssociation();
	//std::cout << "Finished simulating association.\n";
		
	// only 1 iteration of training
	simulationElements.fcpw.trainWeights(10);
	//std::cout << "Finished training weights.\n";
		
	wasRelearningRequested = false;
	hasRelearningFinished = true;
}

void DnfcomposerHandler::updateExternalInput()
{
	static double offset = 1.0;
	GaussStimulusParameters gsp = { 3, 25, 20 };
	gsp.position = simulationParameters.externalInputPosition + offset;
	const std::shared_ptr<GaussStimulus> stimulus
		(new GaussStimulus("stimulus", simulationElements.inputField->getSize(), gsp));

	simulation->addElement(stimulus);
	simulationElements.inputField->addInput(stimulus);
	waitForFieldsToSettle();

	simulation->removeElement("stimulus");
	waitForFieldsToSettle();

	haveFieldsSettled = true;
	wasExternalInputUpdated = false;
}

void DnfcomposerHandler::updateFieldCentroids()
{
	bool userRequestClose = false;
	while (!userRequestClose && !hasExperimentFinished)
	{
		simulationParameters.inputFieldCentroid = simulationElements.inputField->calculateCentroid();
		simulationParameters.outputFieldCentroid = simulationElements.outputField->calculateCentroid();

		if (simulationParameters.isUserInterfaceActive)
			userInterfaceWindow->setCentroids(simulationParameters.inputFieldCentroid, simulationParameters.outputFieldCentroid);

		Sleep(20);
	}
}

// clear methods

void DnfcomposerHandler::clearRelearning()
{
	simulationElements.fcpw.clearTargetPeakLocationsFromFiles();
}

void DnfcomposerHandler::clearDegeneration()
{
	wasDegenerationRequested = false;
}

// other methods

void DnfcomposerHandler::saveWeightsToFile() const
{
	simulationElements.fieldCoupling->saveWeights();
}

void DnfcomposerHandler::waitForFieldsToSettle() const
{
	for (int i = 0; i < simulationParameters.timeForFieldToSettle; i++)
		application->step();
}
