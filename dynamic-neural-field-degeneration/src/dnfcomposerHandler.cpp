

#include "../include/dnfcomposerHandler.h"

DNFComposerHandler::DNFComposerHandler(const std::shared_ptr<Simulation> simulation)
	:simulation(simulation)
{
	visualizations.push_back(std::make_shared<Visualization>(simulation));
	visualizations[0]->addPlottingData("field u", "activation");
	visualizations[0]->addPlottingData("field v", "activation");


	application = std::make_shared<Application>(simulation, visualizations, true);

	inputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement("field u"));
	outputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement("field v"));
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
	double offset = 1.5;
	GaussStimulusParameters gsp = { 3, 15, 20 };
	std::cout << "Stimulus label: " << stimulusLabel << "\n";
	cuboidColorLabel = stimulusLabel;
	gsp.position = cuboidColor[stimulusLabel] + offset;
	std::cout << "Stimulus position: " << gsp.position << "\n";
	std::shared_ptr<GaussStimulus> stimulus(new GaussStimulus("stimulus " + stimulusLabel, 100, gsp));

	simulation->addElement(stimulus);
	inputField->addInput(stimulus);

	simulation->init();
	//visualizations[0]->addPlottingData("stimulus " + stimulusLabel, "output");

	for (int i = 0; i < timeForFieldToSettle; i++)
		application->step();

	
}

std::string DNFComposerHandler::getTargetBox()
{
	double centroid = outputField->calculateCentroid();

	simulation->removeElement("stimulus " + cuboidColorLabel);

	//visualizations[0]->
	//simulation->init();

	double halfRange = 10;

	if (centroid >= 12.5 - halfRange && centroid < 12.5 + halfRange)
		return "BOX_1";
	else if (centroid >= 25 - halfRange && centroid < 25 + halfRange)
		return "BOX_2";
	else if (centroid >= 37.5 - halfRange && centroid < 37.5 + halfRange)
		return "BOX_3";
	else if (centroid >= 50 - halfRange && centroid < 50 + halfRange)
		return "BOX_4";
	else if (centroid >= 62.5 - halfRange && centroid < 62.5 + halfRange)
		return "BOX_5";
	else if (centroid >= 75 - halfRange && centroid < 75 + halfRange)
		return "BOX_6";
	else if (centroid >= 87.5 - halfRange && centroid < 87.5 + halfRange)
		return "BOX_7";
	else
		return "Unknown";
}

void DNFComposerHandler::setupCuboidColorMap()
{
	cuboidColor["RED"] = 12.5;
	cuboidColor["ORANGE"] = 25;
	cuboidColor["YELLOW"] = 37.5;
	cuboidColor["GREEN"] = 50;
	cuboidColor["BLUE"] = 62.5;
	cuboidColor["INDIGO"] = 75;
	cuboidColor["VIOLET"] = 87.5;
}

void DNFComposerHandler::setupTargetBoxMap()
{
	targetBox["BOX_1"] = 12.5;
	targetBox["BOX_2"] = 25;
	targetBox["BOX_3"] = 37.5;
	targetBox["BOX_4"] = 50;
	targetBox["BOX_5"] = 62.5;
	targetBox["BOX_6"] = 75;
	targetBox["BOX_7"] = 87.5;
}
