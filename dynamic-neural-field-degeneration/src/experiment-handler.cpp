
#include "../include/experiment-handler.h"

ExperimentHandler::ExperimentHandler(const ExperimentParameters& params)
	: dnfcomposerHandler(DnfcomposerHandler(params.isComposerVisualizationOn)), params(params)
{
	this->params.setOtherDegeneracyParameters();
	printExperimentParameters();
}

void ExperimentHandler::printExperimentParameters() const
{
	std::cout << "Experiment parameters" << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Number of shapes per trial: " << params.numberOfShapesPerTrial << std::endl;
	std::cout << "Decision tolerance: " << params.decisionTolerance << std::endl;
	std::cout << "Number of trials: " << params.numberOfTrials << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Degeneracy type: " << params.degeneracyName << std::endl;
	std::cout << "Type of elements degenerated: " << params.typeOfElementsDegenerated << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Initial percentage of degeneration: " << params.initialPercentageOfDegeneration << std::endl;
	std::cout << "Target percentage of degeneration: " << params.targetPercentageOfDegeneration << std::endl;
	std::cout << "Increment of degeneration percentage: " << params.incrementOfDegenerationPercentage << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Maximum allowed amount of relearning cycles: " << params.maximumAmountOfRelearningCycles << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Is data saving on: " << (params.isDataSavingOn ? "true" : "false") << std::endl;
	std::cout << "Is dnf-composer visualization on: " << (params.isComposerVisualizationOn ? "true" : "false") << std::endl;
	std::cout << "Is debug mode on: " << (params.isDebugModeOn ? "true" : "false") << std::endl;
	std::cout << "Is link to CoppeliaSim on: " << (params.isLinkToCoppeliaSimOn ? "true" : "false") << std::endl;
	std::cout << "----------------------------------------" << std::endl;
}

void ExperimentHandler::init()
{
	dnfcomposerHandler.init();
	if(params.isLinkToCoppeliaSimOn)
		coppeliasimHandler.init();
	experimentThread = std::thread(&ExperimentHandler::step, this);
}

void ExperimentHandler::step()
{
	if(params.isLinkToCoppeliaSimOn)
		bonafidePickAndPlace();
	else
		mockPickAndPlace();
}

void ExperimentHandler::close()
{
	dnfcomposerHandler.close();
	if (params.isLinkToCoppeliaSimOn)
		coppeliasimHandler.close();
}

bool ExperimentHandler::bonafidePickAndPlace()
{
	stats.shapesPlacedIncorrectly = 0; // binary representation
	bool successfulPickAndPlace = true;

	for (int i = 0; i < params.numberOfShapesPerTrial; i++)
	{
		createShape();
		readShapeHue();
		readTargetAngle();
		if (!verifyDecision())
			successfulPickAndPlace = false;
		graspShape();
		placeShape();
		coppeliasimHandler.resetSignals();
	}

	if(params.isDebugModeOn)
	{
		std::cout << "Binary representation of placed boxes: " << std::bitset<7>(stats.shapesPlacedIncorrectly) << std::endl;
		std::cout << "Pick and place procedure finished, with" << (successfulPickAndPlace ? " success." : "out success.") << std::endl;
	}

	return successfulPickAndPlace;
}

void ExperimentHandler::createShape()
{
	// set the create shape signal to true
	signals.createShape = true;
	coppeliasimHandler.setSignals(signals);
	signals.createShape = false;

	// wait for the shape created signal to be true
	while (!coppeliasimHandler.getSignals().isShapeCreated);
}

void ExperimentHandler::graspShape()
{
	// go pick up the cuboid
	signals.graspShape = true;
	coppeliasimHandler.setSignals(signals);
	signals.graspShape = false;

	// wait for the cuboid to be grasped
	while (!coppeliasimHandler.getSignals().isShapeGrasped);
}

void ExperimentHandler::placeShape()
{
	// and set place shape to true
	signals.placeShape = true;
	coppeliasimHandler.setSignals(signals);
	signals.placeShape = false;
	signals.targetAngle = UNDEFINED;

	// when receive shape placed restart cycle
	while (!coppeliasimHandler.getSignals().isShapePlaced);
	coppeliasimHandler.setSignals(signals);
}

bool ExperimentHandler::verifyDecision()
{
	bool isCorrectDecision = std::abs(data.expectedTargetAngle - data.outputFieldCentroid) <= params.decisionTolerance;
	if (isCorrectDecision)
	{
		stats.shapesPlacedIncorrectly = (stats.shapesPlacedIncorrectly << 1) | 1;
		return true;
	}

	// No matching rules for the given cuboidHue and robotTargetAngle.
	stats.shapesPlacedIncorrectly = stats.shapesPlacedIncorrectly << 1;
	return false;
}

void ExperimentHandler::readShapeHue()
{
	// wait for the hue of the cuboid
	do
	{
		Sleep(50); // necessary?
		signals.shapeHue = coppeliasimHandler.getSignals().shapeHue;
		if (params.isDebugModeOn)
			std::cout << "Shape hue: " << signals.shapeHue << std::endl;
	} while (signals.shapeHue == UNDEFINED);

	// set the hue of the cuboid for dnfcomposer
	dnfcomposerHandler.setExternalInput(signals.shapeHue);
	data.shapeHue = signals.shapeHue;
	signals.shapeHue = UNDEFINED;

	// wait for the shape hue to be read
	while (!dnfcomposerHandler.getHaveFieldsSettled());
	dnfcomposerHandler.setHaveFieldsSettled(false);
}

void ExperimentHandler::readTargetAngle()
{
	signals.targetAngle = dnfcomposerHandler.getOutputFieldCentroid();
	if (params.isDebugModeOn)
		std::cout << "Target angle: " << signals.targetAngle << std::endl;

	data.outputFieldCentroid = signals.targetAngle;
	data.lastOutputFieldCentroid = signals.targetAngle;

	getExpectedTargetAngle();

	// set the target angle for CoppeliaSim
	coppeliasimHandler.setSignals(signals);
}

void ExperimentHandler::getExpectedTargetAngle()
{
	// Check if cuboidHue exists in the map
	auto closestHueIter = hueToAngleMap.end();
	double minDifference = params.decisionTolerance;

	for (auto it = hueToAngleMap.begin(); it != hueToAngleMap.end(); ++it)
	{
		double difference = std::abs(data.shapeHue - it->first);
		if (difference <= params.decisionTolerance && difference < minDifference)
		{
			minDifference = difference;
			closestHueIter = it;
		}
	}

	if (closestHueIter != hueToAngleMap.end())
		data.expectedTargetAngle = closestHueIter->second;

	if (params.isComposerVisualizationOn)
		dnfcomposerHandler.setExpectedFieldBehavior(data.shapeHue, data.expectedTargetAngle);
}

bool ExperimentHandler::mockPickAndPlace()
{
	stats.shapesPlacedIncorrectly = 0; // binary representation
	bool successfulPickAndPlace = true;

	for (int i = 0; i < params.numberOfShapesPerTrial; i++)
	{
		mockReadShapeHue();
		mockReadTargetAngle();
		if (!verifyDecision())
			successfulPickAndPlace = false;
	}

	if (params.isDebugModeOn)
	{
		std::cout << "Binary representation of placed boxes: " << std::bitset<7>(stats.shapesPlacedIncorrectly) << std::endl;
		std::cout << "Pick and place procedure finished, with" << (successfulPickAndPlace ? " success." : "out success.") << std::endl;
	}

	return successfulPickAndPlace;
}

void ExperimentHandler::mockReadShapeHue()
{
	if(hueToAngleIterator == hueToAngleMap.end())
		hueToAngleIterator = hueToAngleMap.begin();

	data.shapeHue = hueToAngleIterator->first;
	if (params.isDebugModeOn)
		std::cout << "External stimulus: " << data.shapeHue << std::endl;
	++hueToAngleIterator;

	dnfcomposerHandler.setExternalInput(data.shapeHue);

	// wait for the shape hue to be read
	while (!dnfcomposerHandler.getHaveFieldsSettled());
	dnfcomposerHandler.setHaveFieldsSettled(false);
}

void ExperimentHandler::mockReadTargetAngle()
{
	signals.targetAngle = dnfcomposerHandler.getOutputFieldCentroid();
	if (params.isDebugModeOn)
		std::cout << "Target angle: " << signals.targetAngle << std::endl;

	data.outputFieldCentroid = signals.targetAngle;
	data.lastOutputFieldCentroid = signals.targetAngle;

	getExpectedTargetAngle();
}


