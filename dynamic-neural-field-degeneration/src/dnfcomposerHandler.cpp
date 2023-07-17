

#include "../include/dnfcomposerHandler.h"

DNFComposerHandler::DNFComposerHandler(const std::shared_ptr<Simulation> simulation)
	:simulation(simulation)
{
	visualization = std::make_shared<Visualization>(simulation);
	visualization->addPlottingData("perceptual field", "activation");
	visualization->addPlottingData("decision field", "activation");


	application = std::make_shared<Application>(simulation, true);

	inputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement("perceptual field"));
	outputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement("decision field"));

	application->activateUserInterfaceWindow(std::make_shared<PlotWindow>(visualization));
	application->activateUserInterfaceWindow(std::make_shared<DegeneracyWindow>(simulation));

	window = std::make_shared<ExperimentWindow>(simulation);
	application->activateUserInterfaceWindow(window);
}

DNFComposerHandler::~DNFComposerHandler()
{
}

void DNFComposerHandler::init()
{
	setupCuboidColorMap();
	setupTargetBoxMap();

	application->init();
}

void DNFComposerHandler::step()
{
	application->step();
	updateStatistics();
	
	window->setCuboidColor(cuboidColor);
	window->setTargetBox(targetBox);
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

void DNFComposerHandler::setExternalStimulus(const std::string& stimulusLabel)
{
	double offset = 1.0;
	GaussStimulusParameters gsp = { 3, 15, 20 };
	std::cout << "Stimulus label: " << stimulusLabel << "\n";
	cuboidColor = stimulusLabel;
	gsp.position = cuboidColorToCentroidMapping[stimulusLabel] + offset;
	std::cout << "Stimulus position: " << gsp.position << "\n";
	std::shared_ptr<GaussStimulus> stimulus(new GaussStimulus("stimulus " + stimulusLabel, 100, gsp));

	simulation->addElement(stimulus);
	inputField->addInput(stimulus);

	simulation->init();
	//visualizations[0]->addPlottingData("stimulus " + stimulusLabel, "output");

	for (int i = 0; i < timeForFieldToSettle; i++)
		application->step();

	simulation->removeElement("stimulus " + cuboidColor);
	
}

std::string DNFComposerHandler::getTargetBox()
{
	double centroid = outputField->calculateCentroid();
	decisionResults.numDecisions++;

	double halfRange = 1;

	if (centroid >= 12.5 - halfRange && centroid < 12.5 + halfRange)
		targetBox = "BOX_1";
	else if (centroid >= 25 - halfRange && centroid < 25 + halfRange)
		targetBox = "BOX_2";
	else if (centroid >= 37.5 - halfRange && centroid < 37.5 + halfRange)
		targetBox = "BOX_3";
	else if (centroid >= 50 - halfRange && centroid < 50 + halfRange)
		targetBox = "BOX_4";
	else if (centroid >= 62.5 - halfRange && centroid < 62.5 + halfRange)
		targetBox = "BOX_5";
	else if (centroid >= 75 - halfRange && centroid < 75 + halfRange)
		targetBox = "BOX_6";
	else if (centroid >= 87.5 - halfRange && centroid < 87.5 + halfRange)
		targetBox = "BOX_7";
	else
		targetBox = "BOX_0";

	verifyOutput();

	return targetBox;
}

void DNFComposerHandler::setupCuboidColorMap()
{
	cuboidColorToCentroidMapping["RED"] = 12.5;
	cuboidColorToCentroidMapping["ORANGE"] = 25;
	cuboidColorToCentroidMapping["YELLOW"] = 37.5;
	cuboidColorToCentroidMapping["GREEN"] = 50;
	cuboidColorToCentroidMapping["BLUE"] = 62.5;
	cuboidColorToCentroidMapping["INDIGO"] = 75;
	cuboidColorToCentroidMapping["VIOLET"] = 87.5;
}

void DNFComposerHandler::setupTargetBoxMap()
{
	targetBoxToCentroidMapping["BOX_1"] = 12.5;
	targetBoxToCentroidMapping["BOX_2"] = 25;
	targetBoxToCentroidMapping["BOX_3"] = 37.5;
	targetBoxToCentroidMapping["BOX_4"] = 50;
	targetBoxToCentroidMapping["BOX_5"] = 62.5;
	targetBoxToCentroidMapping["BOX_6"] = 75;
	targetBoxToCentroidMapping["BOX_7"] = 87.5;
}

void DNFComposerHandler::updateStatistics()
{
	if(decisionResults.numDecisions)
		decisionResults.decisionRatio = (decisionResults.numCorrectDecisions / decisionResults.numDecisions) * 100;
}

void DNFComposerHandler::verifyOutput()
{
	std::string expectedTargetBox;
	std::string actualTargetBox = targetBox;

	// Determine the expected target box based on the current cuboid color
	if (cuboidColor == "RED")
		expectedTargetBox = "BOX_1";
	else if (cuboidColor == "ORANGE")
		expectedTargetBox = "BOX_2";
	else if (cuboidColor == "YELLOW")
		expectedTargetBox = "BOX_3";
	else if (cuboidColor == "GREEN")
		expectedTargetBox = "BOX_4";
	else if (cuboidColor == "BLUE")
		expectedTargetBox = "BOX_5";
	else if (cuboidColor == "INDIGO")
		expectedTargetBox = "BOX_6";
	else if (cuboidColor == "VIOLET")
		expectedTargetBox = "BOX_7";

	// Check if the actual target box matches the expected target box
	if (actualTargetBox == expectedTargetBox)
		decisionResults.numCorrectDecisions++;
	else
		decisionResults.numIncorrectDecisions++;
}
