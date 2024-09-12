
#include "dnfcomposer_handler.h"


DnfcomposerHandler::DnfcomposerHandler()
{
	simulation = getExperimentSimulation();
	application = std::make_unique<dnf_composer::Application>(simulation, true);

	simulationElements.inputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.inputFieldId));
	simulationElements.outputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.outputFieldId));
	simulationElements.fieldCoupling = std::dynamic_pointer_cast<DegenerateFieldCoupling>(simulation->getElement(simulationParameters.fieldCouplingId));

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
			userRequestClose = application->hasUIBeenClosed();
		Sleep(1);
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
	application->addWindow<dnf_composer::user_interface::MainWindow>();
	application->addWindow<imgui_kit::LogWindow>();
	application->addWindow<dnf_composer::user_interface::ElementWindow>();
	application->addWindow<dnf_composer::user_interface::SimulationWindow>();
	application->addWindow<dnf_composer::user_interface::HeatmapWindow>();

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
	//static int count = 0;

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
	//waitForFieldsToSettle();

	/*if(count)
		for (int i = 0; i < 100000; i++)
			application->step();*/

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
}

void DnfcomposerHandler::waitForFieldsToSettle() const
{
	for (int i = 0; i < simulationParameters.timeForFieldToSettle; i++)
		application->step();
}

