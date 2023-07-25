

#include "../include/dnfcomposerHandler.h"

std::unordered_map<double, int> hueToAngleMap = {
		{0.0, 15},
		{40.6, 40},
		{60.0, 65},
		{120.0, 90},
		{240.0, 115},
		{274.150, 140},
		{284.740, 165}
};

DNFComposerHandler::DNFComposerHandler(const std::shared_ptr<Simulation> simulation)
	:simulation(simulation)
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

	window = std::make_shared<ExperimentWindow>(simulation);
	application->activateUserInterfaceWindow(window);
}

DNFComposerHandler::~DNFComposerHandler()
{
}

void DNFComposerHandler::init()
{
	application->init();
}

void DNFComposerHandler::step()
{
	application->step();
	updateStatistics();
	
	//window->setCuboidHue(cuboidHue);
	window->setTargetRobotAngle(targetRobotAngle);
	window->setCurrentTrial(decisionResults.numDecisions);
	window->setNumCorrectDecisions(decisionResults.numCorrectDecisions);
	window->setDecisionRatio(decisionResults.decisionRatio);
}

void DNFComposerHandler::close()
{
	application->close();
}

bool DNFComposerHandler::getUserRequestClose()
{
	return application->getCloseUI();
}

void DNFComposerHandler::setExternalStimulus(const double& cuboidHue)
{
	double offset = 1.0;
	GaussStimulusParameters gsp = { 3, 15, 20 };
	std::cout << "Stimulus label: " << "\n";
	//cuboidColor = stimulusLabel;
	gsp.position = cuboidHue + offset;
	window->setCuboidHue(cuboidHue);

	std::cout << "Stimulus position: " << gsp.position << "\n";
	std::shared_ptr<GaussStimulus> stimulus(new GaussStimulus("stimulus " + cuboidColor, inputField->getSize(), gsp));

	simulation->addElement(stimulus);
	inputField->addInput(stimulus);

	simulation->init();

	for (int i = 0; i < timeForFieldToSettle; i++)
		application->step();

	simulation->removeElement("stimulus " + cuboidColor);
	
}

double DNFComposerHandler::getTargetPlaceAngle()
{
	double centroid = outputField->calculateCentroid();
	decisionResults.numDecisions++;
	targetRobotAngle = centroid;

	verifyOutput();
	return centroid;
}

void DNFComposerHandler::updateStatistics()
{
	if(decisionResults.numDecisions)
		decisionResults.decisionRatio = (decisionResults.numCorrectDecisions / decisionResults.numDecisions) * 100;
}

void DNFComposerHandler::verifyOutput()
{
	if (verifyRobotAngle())
		decisionResults.numCorrectDecisions++;
	else
		decisionResults.numIncorrectDecisions++;
}

bool DNFComposerHandler::verifyRobotAngle()
{
	// Find the closest cuboidHue value in the lookup table.
	double closestHue = cuboidHue;
	double minDistance = std::abs(cuboidHue - closestHue);
	for (const auto& entry : hueToAngleMap) {
		double distance = std::abs(cuboidHue - entry.first);
		if (distance <= 5 && distance < minDistance) {
			closestHue = entry.first;
			minDistance = distance;
		}
	}

	// Check if the corresponding robotTargetAngle matches the provided value within +/- 5.
	auto it = hueToAngleMap.find(closestHue);
	if (it != hueToAngleMap.end()) {
		int targetAngle = it->second;
		return std::abs(targetAngle - targetRobotAngle) <= 5;
	}
}