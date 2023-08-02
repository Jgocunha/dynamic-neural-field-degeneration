
#include "../include/dnfcomposerHandler.h"

DNFComposerHandler::DNFComposerHandler()
{
}

DNFComposerHandler::DNFComposerHandler(const std::shared_ptr<Simulation> simulation, bool isGUIVisible, int timeForFieldToSettle)
	:simulation(simulation), window(std::make_shared<ExperimentWindow>(simulation)), timeForFieldToSettle(timeForFieldToSettle)
{
	// Setup the input and output field.
	inputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement("perceptual field"));
	outputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement("decision field"));
	fieldCoupling = std::dynamic_pointer_cast<DegenerateFieldCoupling>(simulation->getElement("per - dec"));

	// Create the application
	application = std::make_shared<Application>(simulation, isGUIVisible);
	application->activateUserInterfaceWindow(window);
	
	// After creating the application, we can add the windows we want to display.
	// Create visualizations for each plot window and add the plotting data.
	std::shared_ptr<Visualization> visualization = std::make_shared<Visualization>(simulation);
	visualization->addPlottingData("perceptual field", "activation");
	PlotDimensions pd;
	pd = { 0, 360, -25, 30 };
	application->activateUserInterfaceWindow(std::make_shared<PlotWindow>(visualization, pd, false));

	visualization = std::make_shared<Visualization>(simulation);
	visualization->addPlottingData("decision field", "activation");
	pd = { 0, 180, -15, 25 };
	application->activateUserInterfaceWindow(std::make_shared<PlotWindow>(visualization, pd, false));

	application->activateUserInterfaceWindow(std::make_shared<MatrixPlotWindow>(simulation, "per - dec"));

	//centroids.resize(inputField->getSize() * outputField->getSize());
}

DNFComposerHandler::~DNFComposerHandler()
{
}

void DNFComposerHandler::init()
{
	try {
		application->init();
	}
	catch (const std::exception& ex) {
		std::cerr << "Exception in DNFComposerHandler::init(): " << ex.what() << std::endl;
	}
	catch (...) {
		std::cerr << "Unknown exception occurred in DNFComposerHandler::init()" << std::endl;
	}
}

void DNFComposerHandler::step()
{
	try
	{
		application->step();
		updateStatistics();
	
		//window->setCuboidHue(cuboidHue);
		window->setTargetRobotAngle(targetRobotAngle);
		window->setCurrentTrial(decisionResults.numDecisions);
		window->setNumCorrectDecisions(decisionResults.numCorrectDecisions);
		window->setDecisionRatio(decisionResults.decisionRatio);
	}
	catch (const std::exception& ex) {
		std::cerr << "Exception in DNFComposerHandler::step(): " << ex.what() << std::endl;
	}
	catch (...) {
		std::cerr << "Unknown exception occurred in DNFComposerHandler::step()" << std::endl;
	}
}

void DNFComposerHandler::close()
{
	try {
		application->close();
	}
	catch (const std::exception& ex) {
		std::cerr << "Exception in DNFComposerHandler::close(): " << ex.what() << std::endl;
		// Perform any necessary cleanup or fallback behavior
		// ...
	}
	catch (...) {
		std::cerr << "Unknown exception occurred in DNFComposerHandler::close()" << std::endl;
		// Perform any necessary cleanup or fallback behavior
		// ...
	}
}

bool DNFComposerHandler::getUserRequestClose()
{
	return application->getCloseUI();
}

void DNFComposerHandler::setExternalStimulus(const double& cuboidHue)
{
	double offset = 1.0;
	GaussStimulusParameters gsp = { 3, 25, 20 };

	this->cuboidHue = cuboidHue;
	gsp.position = cuboidHue + offset;
	window->setCuboidHue(cuboidHue);

	std::shared_ptr<GaussStimulus> stimulus(new GaussStimulus("stimulus", inputField->getSize(), gsp));

	simulation->addElement(stimulus);
	inputField->addInput(stimulus);

	//simulation->init();

	for (int i = 0; i < timeForFieldToSettle; i++)
		application->step();

	simulation->removeElement("stimulus");

	for (int i = 0; i < timeForFieldToSettle; i++)
		application->step();

	getPerceptualFieldCentroid();
}

void DNFComposerHandler::getPerceptualFieldCentroid()
{
	window->setPerceptualFieldCentroid(inputField->calculateCentroid());
}

double DNFComposerHandler::getTargetPlaceAngle()
{
	targetRobotAngle = outputField->calculateCentroid();
	verifyOutput();
	return targetRobotAngle;
}

void DNFComposerHandler::updateStatistics()
{
	if (decisionResults.numDecisions > 0)
		decisionResults.decisionRatio = (static_cast<double>(decisionResults.numCorrectDecisions) / decisionResults.numDecisions) * 100;
	else
		decisionResults.decisionRatio = 0.0;
}

void DNFComposerHandler::verifyOutput()
{
	decisionResults.numDecisions++;
	// Increment numCorrectDecisions or numIncorrectDecisions based on verifyRobotAngle()
	if (verifyRobotAngle())
		decisionResults.numCorrectDecisions++;
	else
		decisionResults.numIncorrectDecisions++;
	centroids.push_back(targetRobotAngle);
}

bool DNFComposerHandler::verifyRobotAngle()
{
	// Define the tolerance for angle comparison
	const int ANGLE_TOLERANCE = 5;
	// Check if cuboidHue exists in the map
	auto closestHueIter = hueToAngleMap.end();
	double minDifference = ANGLE_TOLERANCE;

	for (auto it = hueToAngleMap.begin(); it != hueToAngleMap.end(); ++it)
	{
		double difference = std::abs(cuboidHue - it->first);
		if (difference <= ANGLE_TOLERANCE && difference < minDifference)
		{
			minDifference = difference;
			closestHueIter = it;
		}
	}

	if (closestHueIter != hueToAngleMap.end())
	{
		double target_angle = closestHueIter->second;
		window->setExpectedTargetAngle(target_angle);
		return std::abs(target_angle - targetRobotAngle) <= ANGLE_TOLERANCE;
	}

	// No matching rules for the given cuboidHue and robotTargetAngle.
	return false;
}

void DNFComposerHandler::applyDegenerationAtTheBeginning(ElementDegeneracyType elementDegeneracyType)
{
	switch (elementDegeneracyType)
	{
		case ElementDegeneracyType::NEURONS_DEACTIVATE_PERCENTAGE:
			inputField->setDegeneracyType(elementDegeneracyType);
			inputField->startDegeneration();
			break;
		case ElementDegeneracyType::WEIGHTS_DEACTIVATE_PERCENTAGE:
		case ElementDegeneracyType::WEIGHTS_RANDOMIZE_PERCENTAGE:
		case ElementDegeneracyType::WEIGHTS_REDUCE_PERCENTAGE: // this is hardcoded to 0.4
			fieldCoupling->setDegeneracyType(elementDegeneracyType);
			fieldCoupling->startDegeneration();
			break;
		default:
			break;
	}

	simulation->step();
}

void DNFComposerHandler::applyDegeneration(ElementDegeneracyType elementDegeneracyType)
{
	switch (elementDegeneracyType)
	{
	case ElementDegeneracyType::NEURONS_DEACTIVATE:
		inputField->setDegeneracyType(elementDegeneracyType);
		inputField->startDegeneration();
		for (int i = 0; i < timeForFieldToSettle; i++)
			application->step();

		getPerceptualFieldCentroid();
		break;
	case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
	case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
	case ElementDegeneracyType::WEIGHTS_REDUCE: // this is hardcoded to 0.4
		fieldCoupling->setDegeneracyType(elementDegeneracyType);
		fieldCoupling->startDegeneration();
		for (int i = 0; i < timeForFieldToSettle; i++)
			application->step();

		getPerceptualFieldCentroid();
		break;
	default:
		break;
	}
}

void DNFComposerHandler::saveCentroids()
{
	std::string filePath = "../../../data/centroids.txt";
	std::ofstream outputFile(filePath, std::ios::app); // Open the file in append mode

	if (!outputFile)
		std::cerr << "Failed to open the file for writing." << std::endl;

	for (const auto& centroid : centroids)
		outputFile << centroid << " ";
	outputFile << std::endl;

	outputFile.close();

	std::cout << "New centroids appended to " << filePath << std::endl;
}