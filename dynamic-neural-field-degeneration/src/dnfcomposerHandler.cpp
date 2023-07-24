

#include "../include/dnfcomposerHandler.h"

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
	pd = { 0, 360, -30, 40 };
	application->activateUserInterfaceWindow(std::make_shared<PlotWindow>(visualizationPer, pd));
	pd = { 0, 180, -30, 40 };
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
	//updateStatistics();
	
	//window->setCuboidColor(cuboidColor);
	//window->setTargetBox(targetBox);
	//window->setCurrentTrial(decisionResults.numDecisions);
	//window->setNumCorrectDecisions(decisionResults.numCorrectDecisions);
	//window->setDecisionRatio(decisionResults.decisionRatio);
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
	std::cout << "Stimulus position: " << gsp.position << "\n";
	std::shared_ptr<GaussStimulus> stimulus(new GaussStimulus("stimulus " + cuboidColor, inputField->getSize(), gsp));

	simulation->addElement(stimulus);
	inputField->addInput(stimulus);

	simulation->init();
	//visualizations[0]->addPlottingData("stimulus " + stimulusLabel, "output");

	for (int i = 0; i < timeForFieldToSettle; i++)
		application->step();

	simulation->removeElement("stimulus " + cuboidColor);
	
}

double DNFComposerHandler::getTargetPlaceAngle()
{
	double centroid = outputField->calculateCentroid();
	//decisionResults.numDecisions++;

	//verifyOutput();

	return centroid;
}

//void DNFComposerHandler::updateStatistics()
//{
//	if(decisionResults.numDecisions)
//		decisionResults.decisionRatio = (decisionResults.numCorrectDecisions / decisionResults.numDecisions) * 100;
//}

//void DNFComposerHandler::verifyOutput()
//{
//	std::string expectedTargetBox;
//	std::string actualTargetBox = targetBox;
//
//	// Determine the expected target box based on the current cuboid color
//	if (cuboidColor == "RED")
//		expectedTargetBox = "BOX_1";
//	else if (cuboidColor == "ORANGE")
//		expectedTargetBox = "BOX_2";
//	else if (cuboidColor == "YELLOW")
//		expectedTargetBox = "BOX_3";
//	else if (cuboidColor == "GREEN")
//		expectedTargetBox = "BOX_4";
//	else if (cuboidColor == "BLUE")
//		expectedTargetBox = "BOX_5";
//	else if (cuboidColor == "INDIGO")
//		expectedTargetBox = "BOX_6";
//	else if (cuboidColor == "VIOLET")
//		expectedTargetBox = "BOX_7";
//
//	// Check if the actual target box matches the expected target box
//	if (actualTargetBox == expectedTargetBox)
//		decisionResults.numCorrectDecisions++;
//	else
//		decisionResults.numIncorrectDecisions++;
//}
