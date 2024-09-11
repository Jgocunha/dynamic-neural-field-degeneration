
#include "dnfcomposer_handler.h"


DnfcomposerHandler::DnfcomposerHandler()
{
	simulation = getExperimentSimulation();
	application = std::make_unique<dnf_composer::Application>(simulation, true);

	simulationElements.inputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.inputFieldId));
	simulationElements.outputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.outputFieldId));
	simulationElements.fieldCoupling = std::dynamic_pointer_cast<DegenerateFieldCoupling>(simulation->getElement(simulationParameters.fieldCouplingId));

	// Addition recovery experiment
	//simulationElements.fcpw = dnf_composer::LearningWizard{ simulation, "per - out" };

	setupUserInterface();
}

DnfcomposerHandler::DnfcomposerHandler(bool isUserInterfaceActive)
{
	simulationParameters.isUserInterfaceActive = isUserInterfaceActive;

	simulation = getExperimentSimulation();
	application = std::make_unique<dnf_composer::Application>(simulation, simulationParameters.isUserInterfaceActive);

	simulationElements.inputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.inputFieldId));
	simulationElements.outputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.outputFieldId));
	simulationElements.fieldCoupling = std::dynamic_pointer_cast<DegenerateFieldCoupling>(simulation->getElement(simulationParameters.fieldCouplingId));

	// Addition recovery experiment
	//simulationElements.fcpw = dnf_composer::LearningWizard{ simulation, "per - out" };

	if(simulationParameters.isUserInterfaceActive)
		setupUserInterface();
}

void DnfcomposerHandler::init()
{
	dnfcomposerThread = std::thread(&DnfcomposerHandler::step, this);
	if(simulationParameters.isUserInterfaceActive) // This is line does not exist in the recovery experiment
		readCentroidsThread = std::thread(&DnfcomposerHandler::updateFieldCentroids, this);
}

void DnfcomposerHandler::step()
{
	application->init();
	
	bool userRequestClose = false;
	while (!userRequestClose && !hasExperimentFinished)
	{
		if(wasDegenerationRequested)
			activateDegeneration();
		else if(wasExternalInputUpdated)
			updateExternalInput();
		else if(hasTrialFinished)
			cleanUpTrial();
		else
			application->step();

		//if (wasDegenerationRequested)
		//	activateDegeneration();
		//else if (wasCloseSimulationRequested)
		//	closeSimulation();
		//else if (wasExternalInputUpdated)
		//	updateExternalInput();
		//else if (wasRelearningRequested)
		//	activateRelearning();
		////else if (wasStartSimulationRequested)
		//	//startSimulation();
		//else if (wasUpdateWeightsRequested)
		//	updateWeights();
		//else if (wasSaveWeightsRequested)
		//	saveWeightsToFile();
		//else
		//	application->step();


		if (simulationParameters.isUserInterfaceActive)
			userRequestClose = application->hasUIBeenClosed();
		Sleep(1);
	}

	application->close();
}

void DnfcomposerHandler::close()
{
	dnfcomposerThread.join();
	if(simulationParameters.isUserInterfaceActive) // This is line does not exist in the recovery experiment
		readCentroidsThread.join();
}

void DnfcomposerHandler::stop()
{
	hasExperimentFinished = true;
}

// New function that is in the recovery experiment
//void DnfcomposerHandler::startSimulation()
//{
//	simulation->init();
//	wasStartSimulationRequested = false;
//}

void DnfcomposerHandler::closeSimulation()
{
	hasTrialFinished = true;

	//numberOfDegeneratedElements = 0;
	//numberOfRelearningCycles = 0;

	//simulationElements.fieldCoupling->readWeights();
	//simulationElements.fieldCoupling->populateIndicesForDegeneration();

	//simulationElements.inputField->clearDegeneration();
	//simulationElements.inputField->populateIndicesForDegeneration();

	//simulationElements.outputField->clearDegeneration();
	//simulationElements.outputField->populateIndicesForDegeneration();

	//wasCloseSimulationRequested = false;
}

// This function is in the recovery experiment
//void DnfcomposerHandler::setRelearningParameters(const RelearningParameters::RelearningType& relearningType, const int& numberOfRelearningEpochs,
//	const double& learningRate, const int& maximumRelearningCycles, const bool updateAllWeights)
//{
//	//log(DEBUG,, "DnfcomposerHandler::setRelearningParameters()\n");
//
//	relearningParameters.relearningType = relearningType;
//	relearningParameters.numberOfRelearningEpochs = numberOfRelearningEpochs;
//	relearningParameters.learningRate = learningRate;
//	relearningParameters.updateAllWeights = updateAllWeights;
//	simulationElements.fieldCoupling->setUpdateAllWeights(updateAllWeights);
//	if (simulationParameters.isUserInterfaceActive)
//		userInterfaceWindow->setRelearningParameters(static_cast<int>(relearningType), numberOfRelearningEpochs, learningRate, maximumRelearningCycles, updateAllWeights);
//}

void DnfcomposerHandler::setDegeneracy(ElementDegeneracyType degeneracyType, const std::string& fieldToDegenerate)
{
	simulationParameters.degeneracyType = degeneracyType;
	simulationParameters.fieldToDegenerate = fieldToDegenerate;
	wasDegenerationRequested = true;
}

void DnfcomposerHandler::setNumberOfElementsToDegenerate(const int& numberOfElementsToDegenerate)
{
	this->numberOfElementsToDegenerate = numberOfElementsToDegenerate;
	simulationElements.fieldCoupling->setNumWeightsToDegenerate(numberOfElementsToDegenerate);
	simulationElements.inputField->setNumNeuronsToDegenerate(numberOfElementsToDegenerate);
}


void DnfcomposerHandler::setExternalInput(const double& position)
{
	this->simulationParameters.externalInputPosition = position;
	wasExternalInputUpdated = true;
}

// This function is in the recovery experiment
//void DnfcomposerHandler::setRelearning(const int& targetRelearningPositions)
//{
//	//log(DEBUG,, "DnfcomposerHandler::setRelearning()\n");
//
//	relearningParameters.targetRelearningPositions = targetRelearningPositions;
//	wasRelearningRequested = true;
//}

void DnfcomposerHandler::setHaveFieldsSettled(bool haveFieldsSettled)
{
	this->haveFieldsSettled = haveFieldsSettled;
}


// This function is in the recovery experiment
//void DnfcomposerHandler::setHasRelearningFinished(bool hasRelearningFinished)
//{
//	//log(DEBUG,, "DnfcomposerHandler::setHasRelearningFinished()\n");
//
//	this->hasRelearningFinished = hasRelearningFinished;
//}

void DnfcomposerHandler::setIsUserInterfaceActiveAs(bool isUserInterfaceActive) const
{
	application->setActivateUserInterfaceAs(isUserInterfaceActive);
}

// This function is in the recovery experiment
//void DnfcomposerHandler::setWasStartSimulationRequested(bool wasStartSimulationRequested)
//{
//	//log(DEBUG,, "DnfcomposerHandler::setWasStartSimulationRequested()\n");
//
//	this->wasStartSimulationRequested = wasStartSimulationRequested;
//}

// This function is in the recovery experiment
//void DnfcomposerHandler::setWasCloseSimulationRequested(bool wasCloseSimulationRequested)
//{
//	//log(DEBUG,, "DnfcomposerHandler::setWasCloseSimulationRequested()\n");
//
//	this->wasCloseSimulationRequested = wasCloseSimulationRequested;
//}

void DnfcomposerHandler::setCentroidDataBeingAccessed(bool isCentroidDataBeingAccessed)
{
	this->isCentroidDataBeingAccessed = isCentroidDataBeingAccessed;
}

double DnfcomposerHandler::getInputFieldCentroid() const
{
	if (!simulationParameters.isUserInterfaceActive)
	{
		return simulationElements.inputField->getCentroid();
	}
	return simulationParameters.inputFieldCentroid;
}

double DnfcomposerHandler::getOutputFieldCentroid() const
{
	if (!simulationParameters.isUserInterfaceActive)
	{
		return simulationElements.outputField->getCentroid();
	}
	return simulationParameters.outputFieldCentroid;
}

bool DnfcomposerHandler::getHaveFieldsSettled() const
{
	return haveFieldsSettled;
}

// This function is in the recovery experiment
//bool DnfcomposerHandler::getHasRelearningFinished() const
//{
//	//log(dnf_composer::DEBUG, "DnfcomposerHandler::getHasRelearningFinished()\n");
//
//	return hasRelearningFinished;
//}

std::shared_ptr<ExperimentWindow> DnfcomposerHandler::getUserInterfaceWindow()
{
	return userInterfaceWindow;
}

// This function is in the recovery experiment
//void DnfcomposerHandler::setIncrementOfDegenerationPercentage(double percentage)
//{
//	//log(DEBUG,, "DnfcomposerHandler::setIncrementOfDegenerationPercentage()\n");
//
//	simulationParameters.incrementOfDegenerationInPercentage = percentage;
//}

void DnfcomposerHandler::initializeFields()
{
	simulation->init();
	wasIntializationRequested = false;
}

void DnfcomposerHandler::setupUserInterface()
{
	application->addWindow<dnf_composer::user_interface::MainWindow>();
	application->addWindow<imgui_kit::LogWindow>();
	application->addWindow<dnf_composer::user_interface::ElementWindow>();
	application->addWindow<dnf_composer::user_interface::SimulationWindow>();
	application->addWindow<dnf_composer::user_interface::HeatmapWindow>();
	//application->addWindow<dnf_composer::user_interface::FieldMetricsWindow>();

	std::shared_ptr<dnf_composer::Visualization> visualization = std::make_shared<dnf_composer::Visualization>(simulation);
	visualization->addPlottingData("perceptual field", "activation");
	visualization->addPlottingData("perceptual field", "output");
	visualization->addPlottingData("per - per", "output");

	dnf_composer::user_interface::PlotParameters pp;
	pp.annotations = { "Perceptual field activation", "Spatial dimension", "Amplitude of activation" };
	pp.dimensions = { 0, 360, -25, 40, 0.5 };
	application->addWindow<dnf_composer::user_interface::PlotWindow>(visualization, pp);

	visualization = std::make_shared<dnf_composer::Visualization>(simulation);
	visualization->addPlottingData("output field", "activation");
	visualization->addPlottingData("output field", "output");
	visualization->addPlottingData("out - out", "output");
	visualization->addPlottingData("per - out", "output");

	pp.annotations = { "Output field activation", "Spatial dimension", "Amplitude of activation" };
	pp.dimensions = { 0, 28, -20, 40, 0.1};
	application->addWindow<dnf_composer::user_interface::PlotWindow>(visualization, pp);

	/*visualization = std::make_shared<dnf_composer::Visualization>(simulation);
	visualization->addPlottingData("per - per", "kernel");
	pp.annotations = { "Kernel_{per}(x-x')", "Spatial dimension", "Amplitude" };
	pp.dimensions = { 0, 125, -1, 1, 0.5};
	application->addWindow<dnf_composer::user_interface::PlotWindow>(visualization, pp);

	visualization = std::make_shared<dnf_composer::Visualization>(simulation);
	visualization->addPlottingData("out - out", "kernel");
	pp.annotations = { "Kernel_{out}(x-x')", "Spatial dimension", "Amplitude" };
	pp.dimensions = { 0, 125, -1, 1, 0.5};
	application->addWindow<dnf_composer::user_interface::PlotWindow>(visualization, pp);*/

	//userInterfaceWindow = std::make_shared<ExperimentWindow>(simulation);
	application->addWindow<ExperimentWindow>();
}

void DnfcomposerHandler::cleanUpTrial()
{
	numberOfDegeneratedElements = 0;
	// call element wise reset here
	const auto inputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.inputFieldId));
	inputField->reset();
	const auto outputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.outputFieldId));
	outputField->reset();
	simulation->close();
	hasTrialFinished = false;
}

void DnfcomposerHandler::updateExternalInput()
{

	/*static auto kernel = std::dynamic_pointer_cast<dnf_composer::element::GaussKernel>(simulation->getElement("per - per"));
	static auto kernel_width = kernel->getParameters().sigma;
	static auto kernel_amplitude = kernel->getParameters().amplitude;

	static double offset = 0.0;
	dnf_composer::element::GaussStimulusParameters gsp = { kernel_width, kernel_amplitude, 20 };
	gsp.position = simulationParameters.externalInputPosition + offset;
	const std::shared_ptr<dnf_composer::element::GaussStimulus> stimulus
	(new dnf_composer::element::GaussStimulus({ "stimulus", {simulationElements.inputField->getMaxSpatialDimension(), simulationElements.inputField->getStepSize()} }, gsp));

	simulation->addElement(stimulus);
	simulationElements.inputField->addInput(stimulus);
	waitForFieldsToSettle();

	simulation->removeElement("stimulus");
	waitForFieldsToSettle();

	haveFieldsSettled = true;
	wasExternalInputUpdated = false;*/

	initializeFields();

	Sleep(100);

	static auto kernel = std::dynamic_pointer_cast<dnf_composer::element::GaussKernel>(simulation->getElement("per - per"));
	static auto kernel_width = kernel->getParameters().width;
	static auto kernel_amplitude = kernel->getParameters().amplitude;

	static double offset = 0.0;
	dnf_composer::element::GaussStimulusParameters gsp = { kernel_width, kernel_amplitude, 20 };
	gsp.position = simulationParameters.externalInputPosition + offset;
	const std::shared_ptr<dnf_composer::element::GaussStimulus> stimulus
	(new dnf_composer::element::GaussStimulus({ "stimulus", {simulationElements.inputField->getMaxSpatialDimension(), simulationElements.inputField->getStepSize()} }, gsp));

	simulation->addElement(stimulus);
	stimulus->init();
	simulationElements.inputField->addInput(stimulus);
	waitForFieldsToSettle();

	simulation->removeElement("stimulus");
	waitForFieldsToSettle();

	haveFieldsSettled = true;
	wasExternalInputUpdated = false;

	//count++;
}

void DnfcomposerHandler::updateFieldCentroids()
{
	bool userRequestClose = false;
	while (!userRequestClose && !hasExperimentFinished)
	{
		simulationParameters.inputFieldCentroid = simulationElements.inputField->getCentroid();
		simulationParameters.outputFieldCentroid = simulationElements.outputField->getCentroid();

		if (simulationParameters.isUserInterfaceActive)
			userRequestClose = application->hasUIBeenClosed();
		Sleep(1);
	}
}

void DnfcomposerHandler::activateDegeneration()
{
	switch (simulationParameters.degeneracyType)
	{
	case ElementDegeneracyType::NEURONS_DEACTIVATE:
		if(simulationParameters.fieldToDegenerate == "perceptual")
		{
			numberOfDegeneratedElements = numberOfDegeneratedElements + numberOfElementsToDegenerate;
			simulationElements.inputField->setDegeneracyType(simulationParameters.degeneracyType);
			simulationElements.inputField->startDegeneration();
		}
		else
		{
			numberOfDegeneratedElements = numberOfDegeneratedElements + numberOfElementsToDegenerate;
			simulationElements.outputField->setDegeneracyType(simulationParameters.degeneracyType);
			simulationElements.outputField->startDegeneration();
		}
		if(simulationParameters.isDebugMode)
			std::cout << "Deactivated " << numberOfDegeneratedElements << " neurons." << std::endl;
		break;
	case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
	case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
	case ElementDegeneracyType::WEIGHTS_REDUCE:
		numberOfDegeneratedElements = numberOfDegeneratedElements + numberOfElementsToDegenerate;
		simulationElements.fieldCoupling->setDegeneracyType(simulationParameters.degeneracyType);
		simulationElements.fieldCoupling->startDegeneration();
		if (simulationParameters.isDebugMode)
			std::cout << "Deactivated " << numberOfDegeneratedElements << " weights." << std::endl;
		break;
	default:
		break;
	}

	waitForFieldsToSettle();

	haveFieldsSettled = true;
	wasDegenerationRequested = false;

	//switch (simulationParameters.degeneracyType)
	//{
	//case dnf_composer::element::ElementDegeneracyType::NEURONS_DEACTIVATE:
	//	if (simulationParameters.fieldToDegenerate == "perceptual")
	//	{
	//		simulationElements.inputField->setDegeneracyType(simulationParameters.degeneracyType);
	//		simulationElements.inputField->startDegeneration();
	//		log(dnf_composer::INFO, "Degenerating the perceptual field.\n");
	//	}
	//	else
	//	{
	//		simulationElements.outputField->setDegeneracyType(simulationParameters.degeneracyType);
	//		simulationElements.outputField->startDegeneration();
	//		log(dnf_composer::INFO, "Degenerating the output field.\n");
	//	}
	//	break;
	//case dnf_composer::element::ElementDegeneracyType::WEIGHTS_DEACTIVATE:
	//case dnf_composer::element::ElementDegeneracyType::WEIGHTS_RANDOMIZE:
	//case dnf_composer::element::ElementDegeneracyType::WEIGHTS_REDUCE:
	//	simulationElements.fieldCoupling->setDegeneracyType(simulationParameters.degeneracyType);
	//	simulationElements.fieldCoupling->startDegeneration();
	//	log(dnf_composer::INFO, "Degenerating the field coupling.\n");
	//	break;
	//default:
	//	break;
	//}

	//waitForFieldsToSettle();

	//if (simulationParameters.isUserInterfaceActive)
	//	userInterfaceWindow->setNumberOfDegeneratedElements(numberOfDegeneratedElements);

	//haveFieldsSettled = true;
	//wasDegenerationRequested = false;
}

void DnfcomposerHandler::waitForFieldsToSettle() const
{
	for (int i = 0; i < simulationParameters.timeForFieldToSettle; i++)
		application->step();
}

// This is in the recovery experiment
//void DnfcomposerHandler::activateRelearning()
//{
//	//log(DEBUG,, "DnfcomposerHandler::activateRelearning()\n");
//
//	// NOW WE HAVE RELEARNING TYPE, LEARNING RATE, AND NUMBER OF ITERATIONS
//	// USE THIS INFORMATION TO TRAIN THE WEIGHTS
//	simulationElements.fieldCoupling->setLearningRate(relearningParameters.learningRate);
//
//	// Remove previously written target peak locations from files
//	simulationElements.fcpw.clearTargetPeakLocationsFromFiles();
//
//	switch (relearningParameters.relearningType)
//	{
//	case RelearningParameters::RelearningType::ALL_CASES:
//		allCasesRelearning();
//		break;
//	case RelearningParameters::RelearningType::ONLY_DEGENERATED_CASES:
//		onlyDegeneratedCasesRelearning();
//		break;
//	default:
//		break;
//	}
//
//	simulationElements.fcpw.simulateAssociation();
//	//std::cout << "Finished simulating association.\n";
//
//	simulationElements.fcpw.trainWeights(relearningParameters.numberOfRelearningEpochs);
//	//std::cout << "Finished training weights.\n";
//
//	wasRelearningRequested = false;
//	hasRelearningFinished = true;
//}

// This is in the recovery experiment
//void DnfcomposerHandler::readWeights()
//{
//	//log(DEBUG,, "DnfcomposerHandler::readWeights()\n");
//
//	wasUpdateWeightsRequested = true;
//}

// This is in the recovery experiment
//void DnfcomposerHandler::updateWeights()
//{
//	//log(DEBUG,, "DnfcomposerHandler::updateWeights()\n");
//
//	simulationElements.fieldCoupling->readWeights();
//	wasUpdateWeightsRequested = false;
//}

// This is in the recovery experiment
//void DnfcomposerHandler::setDataFilePath(const std::string& filePath)
//{
//	//log(DEBUG,, "DnfcomposerHandler::setDataFilePath()\n");
//
//	simulationElements.fieldCoupling->setWeightsFilePath(filePath);
//	simulationElements.fcpw.setDataFilePath(filePath);
//}

// This function is in the recovery experiment
//void DnfcomposerHandler::saveWeights()
//{
//	//log(DEBUG,, "DnfcomposerHandler::saveWeights()\n");
//
//	wasSaveWeightsRequested = true;
//}
//
//
//void DnfcomposerHandler::saveWeightsToFile()
//{
//	//log(DEBUG,, "DnfcomposerHandler::saveWeightsToFile()\n");
//
//	simulationElements.fieldCoupling->saveWeights();
//	wasSaveWeightsRequested = false;
//}

// This function is in the recovery experiment
//void DnfcomposerHandler::allCasesRelearning()
//{
//	//log(DEBUG,, "DnfcomposerHandler::allCasesRelearning()\n");
//
//	// add gaussian inputs
//	//dnf_composer::element::GaussStimulusParameters gsp = { 3, 35, 20 };
//
//	simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPre(inputTargetPeaksForCoupling);
//	simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPost(outputTargetPeaksForCoupling);
//	//std::cout << "Finished setting up the field coupling wizard.\n";
//
//
//	//gsp.amplitude = 35;
//	//gsp.sigma = 3;
//	//simulationElements.fcpw.setGaussStimulusParameters(gsp);
//	////log(dnf_composer::WARNING, "allCasesRelearning() finished setting target peak locations");
//
//	//std::cout << "Finished setting up the gaussian stimulus parameters.\n";
//
//}

// This function is in the recovery experiment
//void DnfcomposerHandler::onlyDegeneratedCasesRelearning()
//{
//	//log(dnf_composer::DEBUG, "DnfcomposerHandler::onlyDegeneratedCasesRelearning()\n");
//
//	// add gaussian inputs
//	dnf_composer::element::GaussStimulusParameters gsp = { 3, 35, 20 };
//
//	std::vector<std::vector<double>> inputSelected;
//	std::vector<std::vector<double>> outputSelected;
//
//	std::ostringstream logStream;
//
//
//	logStream << "Target behaviors to relearn ";
//
//	for (int i = 0; i < inputTargetPeaksForCoupling.size(); i++)
//	{
//		if (!(relearningParameters.targetRelearningPositions & (1 << i)))
//		{
//			//int index = 6 - i;
//			//if (index == 2)
//			//	index = 4;
//			//else
//			//	if (index == 4)
//			//		index = 2;
//			constexpr int index = 0;
//			log(dnf_composer::WARNING, "onlyDegeneratedCasesRelearning() indexing is hardcoded.\n");
//
//
//			inputSelected.push_back(inputTargetPeaksForCoupling[index]);
//			outputSelected.push_back(outputTargetPeaksForCoupling[index]);
//			logStream << outputTargetPeaksForCoupling[index][0] - offset << " ";
//
//		}
//	}
//
//	logStream << std::endl;
//
//	dnf_composer::log(dnf_composer::INFO, logStream.str());
//
//	simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPre(inputSelected);
//	simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPost(outputSelected);
//	//std::cout << "Finished setting up the field coupling wizard.\n";
//
//
//
//	//gsp.amplitude = 35;
//	//gsp.sigma = 3;
//	//simulationElements.fcpw.setGaussStimulusParameters(gsp);
//	////std::cout << "Finished setting up the gaussian stimulus parameters.\n";
//
//}
//
//
//void DnfcomposerHandler::setIncrementOfDegenerationPercentage(double percentage)
//{
//	//log(DEBUG,, "DnfcomposerHandler::setIncrementOfDegenerationPercentage()\n");
//
//	simulationParameters.incrementOfDegenerationInPercentage = percentage;
//}
//
//
//void DnfcomposerHandler::setInitialNumberOfElementsToDegenerate(int count) const
//{
//	//log(DEBUG,, "DnfcomposerHandler::setInitialNumberOfElementsToDegenerate()\n");
//
//	simulationElements.inputField->setNumNeuronsToDegenerate(count);
//	simulationElements.outputField->setNumNeuronsToDegenerate(count);
//	simulationElements.fieldCoupling->setNumWeightsToDegenerate(count);
//}
//
//
//void DnfcomposerHandler::setNumberOfElementsToDegenerate() const
//{
//	//log(DEBUG,, "DnfcomposerHandler::setNumberOfElementsToDegenerate()\n");
//
//	int numberOfElements;
//	double floatingNumberOfElements;
//
//	switch (simulationParameters.degeneracyType)
//	{
//	case dnf_composer::element::ElementDegeneracyType::NEURONS_DEACTIVATE:
//		if (simulationParameters.fieldToDegenerate == "perceptual")
//		{
//			// Perform the calculation 			// Round the result to the nearest integer
//			floatingNumberOfElements = static_cast<double>(simulationElements.inputField->getSize()) / 100 * simulationParameters.incrementOfDegenerationInPercentage;
//			numberOfElements = static_cast<int>(std::round(floatingNumberOfElements));
//			simulationElements.inputField->setNumNeuronsToDegenerate(numberOfElements);
//			log(dnf_composer::INFO, " Number of pre-synaptic neurons to degenerate in each iteration: " + std::to_string(numberOfElements) + ".\n");
//		}
//		if (simulationParameters.fieldToDegenerate == "output")
//		{
//			// Perform the calculation 			// Round the result to the nearest integer
//			floatingNumberOfElements = static_cast<double>(simulationElements.outputField->getSize()) / 100 * simulationParameters.incrementOfDegenerationInPercentage;
//			numberOfElements = static_cast<int>(std::round(floatingNumberOfElements));
//			simulationElements.outputField->setNumNeuronsToDegenerate(numberOfElements);
//			log(dnf_composer::INFO, "Number of post-synaptic neurons to degenerate in each iteration: " + std::to_string(numberOfElements) + ".\n");
//		}
//		break;
//
//	case dnf_composer::element::ElementDegeneracyType::WEIGHTS_DEACTIVATE:
//	case dnf_composer::element::ElementDegeneracyType::WEIGHTS_RANDOMIZE:
//	case dnf_composer::element::ElementDegeneracyType::WEIGHTS_REDUCE:
//		// Perform the calculation 			// Round the result to the nearest integer
//		floatingNumberOfElements = static_cast<double>(simulationElements.inputField->getSize() * simulationElements.outputField->getSize()) / 100 * simulationParameters.incrementOfDegenerationInPercentage;
//		numberOfElements = static_cast<int>(std::round(floatingNumberOfElements));
//		simulationElements.fieldCoupling->setNumWeightsToDegenerate(numberOfElements);
//		log(dnf_composer::INFO, " Number of inter-synaptic connections to degenerate in each iteration: " + std::to_string(numberOfElements) + ".\n");
//		break;
//	default:
//		break;
//	}
//}
//
//int DnfcomposerHandler::getNumberOfDegeneratedElements()
//{
//	// return etc.
//	return simulationElements.fieldCoupling->getNumIndicesForDegeneration();
//}

