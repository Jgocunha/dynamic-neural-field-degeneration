
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
	//readCentroidsThread = std::thread(&DnfcomposerHandler::updateFieldCentroids, this);
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
		else if (wasStartSimulationRequested)
			startSimulation();
		else if(wasCloseSimulationRequested)
			closeSimulation();
		else
			application->step();

		Sleep(1);
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

void DnfcomposerHandler::startSimulation()
{
	simulation->init();
	wasStartSimulationRequested = false;
}

void DnfcomposerHandler::closeSimulation()
{
	numberOfDegeneratedElements = 0;
	numberOfRelearningCycles = 0;
	simulationElements.fieldCoupling->readWeights();
	simulationElements.fieldCoupling->populateIndicesForDegeneration();
	wasCloseSimulationRequested = false;
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

void DnfcomposerHandler::setRelearningCycles(const int& relearningCycles) const
{
	if (simulationParameters.isUserInterfaceActive)
		userInterfaceWindow->setRelearningCycles(relearningCycles);
}

void DnfcomposerHandler::setRelearningParameters(const RelearningParameters::RelearningType& relearningType, const int& numberOfRelearningEpochs, 
	const double& learningRate, const int& maximumRelearningCycles, const bool updateAllWeights)
{
	relearningParameters.relearningType = relearningType;
	relearningParameters.numberOfRelearningEpochs = numberOfRelearningEpochs;
	relearningParameters.learningRate = learningRate;
	relearningParameters.updateAllWeights = updateAllWeights;
	simulationElements.fieldCoupling->setUpdateAllWeights(updateAllWeights);
	if (simulationParameters.isUserInterfaceActive)
		userInterfaceWindow->setRelearningParameters(static_cast<int>(relearningType), numberOfRelearningEpochs, learningRate, maximumRelearningCycles, updateAllWeights);
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

void DnfcomposerHandler::setRelearning(const int& targetRelearningPositions)
{
	relearningParameters.targetRelearningPositions = targetRelearningPositions;
	wasRelearningRequested = true;
}

void DnfcomposerHandler::setHaveFieldsSettled(bool haveFieldsSettled)
{
	this->haveFieldsSettled = haveFieldsSettled;
}

void DnfcomposerHandler::setHasRelearningFinished(bool hasRelearningFinished)
{
	this->hasRelearningFinished = hasRelearningFinished;
}

void DnfcomposerHandler::setIsUserInterfaceActiveAs(bool isUserInterfaceActive) const
{
	application->setActivateUserInterfaceAs(isUserInterfaceActive);
}

void DnfcomposerHandler::setWasStartSimulationRequested(bool wasStartSimulationRequested)
{
	this->wasStartSimulationRequested = wasStartSimulationRequested;
}

void DnfcomposerHandler::setWasCloseSimulationRequested(bool wasCloseSimulationRequested)
{
	this->wasCloseSimulationRequested = wasCloseSimulationRequested;
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

	switch (simulationParameters.degeneracyType)
	{
	case ElementDegeneracyType::NEURONS_DEACTIVATE:
		numberOfDegeneratedElements = numberOfDegeneratedElements + 1; // hardcoded to 1
		if (simulationParameters.fieldToDegenerate == "perceptual")
		{
			simulationElements.inputField->setDegeneracyType(simulationParameters.degeneracyType);
			simulationElements.inputField->startDegeneration();
		}
		else
		{
			simulationElements.outputField->setDegeneracyType(simulationParameters.degeneracyType);
			simulationElements.outputField->startDegeneration();
		}
		if (simulationParameters.isDebugMode)
			std::cout << "Deactivated " << numberOfDegeneratedElements << " neurons." << std::endl;
		break;
	case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
	case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
	case ElementDegeneracyType::WEIGHTS_REDUCE: // this is hardcoded to 0.4
		numberOfDegeneratedElements = numberOfDegeneratedElements + 100; // hardcoded to 100
		simulationElements.fieldCoupling->setDegeneracyType(simulationParameters.degeneracyType);
		simulationElements.fieldCoupling->startDegeneration();
		if (simulationParameters.isDebugMode)
			std::cout << "Deactivated " << numberOfDegeneratedElements << " weights." << std::endl;
		break;
	default:
		break;
	}

	waitForFieldsToSettle();

	if (simulationParameters.isUserInterfaceActive)
		userInterfaceWindow->setNumberOfDegeneratedElements(numberOfDegeneratedElements);

	haveFieldsSettled = true;
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
	// NOW WE HAVE RELEARNING TYPE, LEARNING RATE, AND NUMBER OF ITERATIONS
	// USE THIS INFORMATION TO TRAIN THE WEIGHTS
	simulationElements.fieldCoupling->setLearningRate(relearningParameters.learningRate);

	// Remove previously written target peak locations from files
	simulationElements.fcpw.clearTargetPeakLocationsFromFiles();

	switch(relearningParameters.relearningType)
	{
		case RelearningParameters::RelearningType::ALL_CASES:
			allCasesRelearning();
		break;
		case RelearningParameters::RelearningType::ONLY_DEGENERATED_CASES:
			onlyDegeneratedCasesRelearning();
		break;
		default:
		break;
	}

	simulationElements.fcpw.simulateAssociation();
	//std::cout << "Finished simulating association.\n";

	simulationElements.fcpw.trainWeights(relearningParameters.numberOfRelearningEpochs);
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
	simulationParameters.inputFieldCentroid = simulationElements.inputField->calculateCentroid();
	simulationParameters.outputFieldCentroid = simulationElements.outputField->calculateCentroid();

	if (simulationParameters.isUserInterfaceActive)
		userInterfaceWindow->setCentroids(simulationParameters.inputFieldCentroid, simulationParameters.outputFieldCentroid);
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

void DnfcomposerHandler::allCasesRelearning()
{
	// add gaussian inputs
	GaussStimulusParameters gsp = { 3, 25, 20 };

	simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPre(inputTargetPeaksForCoupling);
	simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPost(outputTargetPeaksForCoupling);
	//std::cout << "Finished setting up the field coupling wizard.\n";

	gsp.amplitude = 25;
	gsp.sigma = 3;
	simulationElements.fcpw.setGaussStimulusParameters(gsp);
	//std::cout << "Finished setting up the gaussian stimulus parameters.\n";

}

void DnfcomposerHandler::onlyDegeneratedCasesRelearning()
{
	// add gaussian inputs
	GaussStimulusParameters gsp = { 3, 25, 20 };

	std::vector<std::vector<double>> inputSelected;
	std::vector<std::vector<double>> outputSelected;

	for (int i = 0; i < inputTargetPeaksForCoupling.size(); ++i) 
	{
		if (!(relearningParameters.targetRelearningPositions & (1 << i))) 
		{
			inputSelected.push_back(inputTargetPeaksForCoupling[i]);
			outputSelected.push_back(outputTargetPeaksForCoupling[i]);
		}
	}

	simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPre(inputSelected);
	simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPost(outputSelected);
	//std::cout << "Finished setting up the field coupling wizard.\n";

	gsp.amplitude = 25;
	gsp.sigma = 3;
	simulationElements.fcpw.setGaussStimulusParameters(gsp);
	//std::cout << "Finished setting up the gaussian stimulus parameters.\n";

}
