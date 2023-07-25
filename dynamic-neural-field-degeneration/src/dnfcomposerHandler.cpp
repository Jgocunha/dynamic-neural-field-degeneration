

#include "../include/dnfcomposerHandler.h"



DNFComposerHandler::DNFComposerHandler()
{
}

DNFComposerHandler::DNFComposerHandler(const std::shared_ptr<Simulation> simulation)
	:simulation(simulation),
	window(std::make_shared<ExperimentWindow>(simulation))
{
	visualizationPer = std::make_shared<Visualization>(simulation);
	visualizationDec = std::make_shared<Visualization>(simulation);

	visualizationPer->addPlottingData("perceptual field", "activation");
	visualizationDec->addPlottingData("decision field", "activation");


	application = std::make_shared<Application>(simulation, true);

	// After creating the application, we can add the windows we want to display.
	application->activateUserInterfaceWindow(std::make_shared<SimulationWindow>(simulation));
	PlotDimensions pd;
	pd = { 0, 360, -20, 20 };
	application->activateUserInterfaceWindow(std::make_shared<PlotWindow>(visualizationPer, pd));
	pd = { 0, 180, -20, 20 };
	application->activateUserInterfaceWindow(std::make_shared<PlotWindow>(visualizationDec, pd));
	application->activateUserInterfaceWindow(std::make_shared<DegeneracyWindow>(simulation));

	inputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement("perceptual field"));
	outputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement("decision field"));

	application->activateUserInterfaceWindow(window);
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
	GaussStimulusParameters gsp = { 3, 15, 20 };

	gsp.position = cuboidHue + offset;
	window->setCuboidHue(cuboidHue);

	std::cout << "Stimulus position: " << gsp.position << "\n";
	std::shared_ptr<GaussStimulus> stimulus(new GaussStimulus("stimulus", inputField->getSize(), gsp));

	simulation->addElement(stimulus);
	inputField->addInput(stimulus);

	simulation->init();

	for (int i = 0; i < timeForFieldToSettle; i++)
		application->step();

	simulation->removeElement("stimulus");
	
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
	verifyRobotAngle() ? decisionResults.numCorrectDecisions++ : decisionResults.numIncorrectDecisions++;
}

bool DNFComposerHandler::verifyRobotAngle()
{
	// Use binary search to find the closest hue value in the lookup table.
	auto it = std::lower_bound(hueToAngleMap.begin(), hueToAngleMap.end(), cuboidHue,
		[](const auto& entry, double hue) {
			return entry.first < hue;
		});

	if (it != hueToAngleMap.end()) {
		double closestHue = it->first;
		double minDistance = std::abs(cuboidHue - closestHue);

		// Check if the corresponding robotTargetAngle matches the provided value within +/- 5.
		int targetAngle = it->second;
		if (std::abs(targetAngle - targetRobotAngle) <= 5) {
			return true;
		}

		// Check the previous element if it is closer in distance
		if (it != hueToAngleMap.begin()) {
			auto prevIt = std::prev(it);
			double prevHue = prevIt->first;
			double prevDistance = std::abs(cuboidHue - prevHue);
			if (prevDistance < minDistance && std::abs(prevIt->second - targetRobotAngle) <= 5) {
				return true;
			}
		}
	}

	return false;
}