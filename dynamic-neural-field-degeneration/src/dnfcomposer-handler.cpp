
#include "../include/dnfcomposer-handler.h"

#include "user_interface/plot_window.h"


DnfcomposerHandler::DnfcomposerHandler()
{
	simulation = getExperimentSimulation();
	application = std::make_unique<dnf_composer::Application>(simulation, true);

	simulationElements.inputField = std::dynamic_pointer_cast<dnf_composer::DegenerateNeuralField>(simulation->getElement(simulationParameters.inputFieldId));
	simulationElements.outputField = std::dynamic_pointer_cast<dnf_composer::DegenerateNeuralField>(simulation->getElement(simulationParameters.outputFieldId));
	simulationElements.fieldCoupling = std::dynamic_pointer_cast<dnf_composer::DegenerateFieldCoupling>(simulation->getElement(simulationParameters.fieldCouplingId));

	setupUserInterface();
}

DnfcomposerHandler::DnfcomposerHandler(bool isUserInterfaceActive)
{
	simulationParameters.isUserInterfaceActive = isUserInterfaceActive;

	simulation = getExperimentSimulation();
	application = std::make_unique<dnf_composer::Application>(simulation, simulationParameters.isUserInterfaceActive);

	simulationElements.inputField = std::dynamic_pointer_cast<dnf_composer::DegenerateNeuralField>(simulation->getElement(simulationParameters.inputFieldId));
	simulationElements.outputField = std::dynamic_pointer_cast<dnf_composer::DegenerateNeuralField>(simulation->getElement(simulationParameters.outputFieldId));
	simulationElements.fieldCoupling = std::dynamic_pointer_cast<dnf_composer::DegenerateFieldCoupling>(simulation->getElement(simulationParameters.fieldCouplingId));

	if(simulationParameters.isUserInterfaceActive)
		setupUserInterface();
}

void DnfcomposerHandler::init()
{
	dnfcomposerThread = std::thread(&DnfcomposerHandler::step, this);
	if(simulationParameters.isUserInterfaceActive)
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

		if (simulationParameters.isUserInterfaceActive)
			userRequestClose = application->getCloseUI();
		Sleep(5);
	}

	application->close();
}

void DnfcomposerHandler::close()
{
	dnfcomposerThread.join();
	if(simulationParameters.isUserInterfaceActive)
		readCentroidsThread.join();
}

void DnfcomposerHandler::stop()
{
	hasExperimentFinished = true;
}

void DnfcomposerHandler::closeSimulation()
{
	hasTrialFinished = true;
}

void DnfcomposerHandler::setDegeneracy(dnf_composer::element::ElementDegeneracyType degeneracyType, const std::string& fieldToDegenerate)
{
	simulationParameters.degeneracyType = degeneracyType;
	simulationParameters.fieldToDegenerate = fieldToDegenerate;
	wasDegenerationRequested = true;
}

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

void DnfcomposerHandler::setExternalInput(const double& position)
{
	this->simulationParameters.externalInputPosition = position;
	wasExternalInputUpdated = true;
}

void DnfcomposerHandler::setHaveFieldsSettled(bool haveFieldsSettled)
{
	this->haveFieldsSettled = haveFieldsSettled;
}

void DnfcomposerHandler::setIsUserInterfaceActiveAs(bool isUserInterfaceActive) const
{
	application->setActivateUserInterfaceAs(isUserInterfaceActive);
}

void DnfcomposerHandler::setCentroidDataBeingAccessed(bool isCentroidDataBeingAccessed)
{
	this->isCentroidDataBeingAccessed = isCentroidDataBeingAccessed;
}

double DnfcomposerHandler::getInputFieldCentroid() const
{
	if (!simulationParameters.isUserInterfaceActive)
		return simulationElements.inputField->getCentroid();
	return simulationParameters.inputFieldCentroid;
}

double DnfcomposerHandler::getOutputFieldCentroid() const
{
	if(!simulationParameters.isUserInterfaceActive)
		return simulationElements.outputField->getCentroid();
	return simulationParameters.outputFieldCentroid;
}

bool DnfcomposerHandler::getHaveFieldsSettled() const
{
	return haveFieldsSettled;
}

std::shared_ptr<ExperimentWindow> DnfcomposerHandler::getUserInterfaceWindow()
{
	return userInterfaceWindow;
}

void DnfcomposerHandler::initializeFields()
{
	simulation->init();
	wasIntializationRequested = false;
}

void DnfcomposerHandler::setupUserInterface()
{
	std::shared_ptr<dnf_composer::Visualization> visualization = std::make_shared<dnf_composer::Visualization>(simulation);
	visualization->addPlottingData("perceptual field", "activation");
	visualization->addPlottingData("per - per", "output");

	dnf_composer::user_interface::PlotParameters pp;
	pp.annotations = { "Perceptual field activation", "Spatial dimension", "Amplitude of activation" };
	pp.dimensions = { 0, 360, -35, 30 };
	application->activateUserInterfaceWindow(std::make_shared<dnf_composer::user_interface::PlotWindow>(visualization, pp));

	visualization = std::make_shared<dnf_composer::Visualization>(simulation);
	visualization->addPlottingData("decision field", "activation");
	visualization->addPlottingData("dec - dec", "output");

	pp.annotations = { "Decision field activation", "Spatial dimension", "Amplitude of activation" };
	pp.dimensions = { 0, 28, -20, 25 };
	application->activateUserInterfaceWindow(std::make_shared<dnf_composer::user_interface::PlotWindow>(visualization, pp));

	application->activateUserInterfaceWindow(std::make_shared<dnf_composer::user_interface::LoggerWindow>());

	userInterfaceWindow = std::make_shared<ExperimentWindow>(simulation);
	application->activateUserInterfaceWindow(userInterfaceWindow);



	//application->activateUserInterfaceWindow(std::make_shared<MatrixPlotWindow>(simulation, "per - dec"));
}

void DnfcomposerHandler::cleanUpTrial()
{
	numberOfDegeneratedElements = 0;
	simulation->close();
	hasTrialFinished = false;
}

void DnfcomposerHandler::updateExternalInput()
{
	initializeFields();

	Sleep(100);

	static double offset = 1.0;
	dnf_composer::element::GaussStimulusParameters gsp = { 3, 15, 20 };
	gsp.position = simulationParameters.externalInputPosition + offset;
	const std::shared_ptr<dnf_composer::element::GaussStimulus> stimulus
		(new dnf_composer::element::GaussStimulus("stimulus", simulationElements.inputField->getSize(), gsp));

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
		simulationParameters.inputFieldCentroid = simulationElements.inputField->getCentroid();
		simulationParameters.outputFieldCentroid = simulationElements.outputField->getCentroid();

		if(simulationParameters.isUserInterfaceActive)
			userInterfaceWindow->setCentroids(simulationParameters.inputFieldCentroid, simulationParameters.outputFieldCentroid);

		if(simulationParameters.isUserInterfaceActive)
			userRequestClose = application->getCloseUI();
		Sleep(5);
	}
}

void DnfcomposerHandler::activateDegeneration()
{
	switch (simulationParameters.degeneracyType)
	{
	case dnf_composer::element::ElementDegeneracyType::NEURONS_DEACTIVATE:
		if(simulationParameters.fieldToDegenerate == "perceptual")
		{
			numberOfDegeneratedElements = numberOfDegeneratedElements + 1;
			simulationElements.inputField->setDegeneracyType(simulationParameters.degeneracyType);
			simulationElements.inputField->startDegeneration();
		}
		else
		{
			numberOfDegeneratedElements = numberOfDegeneratedElements + 1;
			simulationElements.outputField->setDegeneracyType(simulationParameters.degeneracyType);
			simulationElements.outputField->startDegeneration();
		}
		if(simulationParameters.isDebugMode)
			std::cout << "Deactivated " << numberOfDegeneratedElements << " neurons." << std::endl;
		break;
	case dnf_composer::element::ElementDegeneracyType::WEIGHTS_DEACTIVATE:
	case dnf_composer::element::ElementDegeneracyType::WEIGHTS_RANDOMIZE:
	case dnf_composer::element::ElementDegeneracyType::WEIGHTS_REDUCE:
		numberOfDegeneratedElements = numberOfDegeneratedElements + 10;
		simulationElements.fieldCoupling->setDegeneracyType(simulationParameters.degeneracyType);
		simulationElements.fieldCoupling->startDegeneration();
		if (simulationParameters.isDebugMode)
			std::cout << "Deactivated " << numberOfDegeneratedElements << " weights." << std::endl;
		break;
	default:
		break;
	}

	waitForFieldsToSettle();

	if(simulationParameters.isUserInterfaceActive)
		userInterfaceWindow->setNumberOfDegeneratedElements(numberOfDegeneratedElements);

	haveFieldsSettled = true;
	wasDegenerationRequested = false;
}

void DnfcomposerHandler::waitForFieldsToSettle() const
{
	for (int i = 0; i < simulationParameters.timeForFieldToSettle; i++)
		application->step();
}

