
#include "../include/experiment-handler.h"

ExperimentHandler::ExperimentHandler(const ExperimentParameters& param)
	:param(param)
{
}

void ExperimentHandler::init()
{
	dnfcomposerHandler.init();
	coppeliasimHandler.init();
	experimentThread = std::thread(&ExperimentHandler::step, this);
}

void ExperimentHandler::step()
{
	// do a complete task
	pickAndPlace();

	// degenerate
	degenerationProcedure();

	// do re-learning cycle
	for (int i = 0; i < param.numberOfTrials; i++)
		pickAndPlaceWithLearning();
}

void ExperimentHandler::close()
{
	dnfcomposerHandler.close();
	coppeliasimHandler.close();
}

void ExperimentHandler::pickAndPlace()
{
	for (int i = 0; i < param.numberOfShapesPerTrial; i++)
	{
		createShape();
		readShapeHue();
		graspShape();
		readTargetAngle();
		verifyDecision();
		placeShape();
		updateStatistics();
		cleanUpTrial();
	}
}

void ExperimentHandler::pickAndPlaceWithLearning()
{
	for (int i = 0; i < param.numberOfShapesPerTrial; i++)
	{
		createShape();
		readShapeHue();
		graspShape();
		readTargetAngle();
		if (!verifyDecision())
			relearningProcedure();
		placeShape();
		updateStatistics();
		cleanUpTrial();
	}
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
}

void ExperimentHandler::readShapeHue()
{
	// wait for the hue of the cuboid
	do
		signals.shapeHue = coppeliasimHandler.getSignals().shapeHue;
	while (signals.shapeHue == UNDEFINED);

	// set the hue of the cuboid for dnfcomposer
	dnfcomposerHandler.setExternalInput(signals.shapeHue);
	data.shapeHue = signals.shapeHue; // !! calcute the expected target angle here
	signals.shapeHue = UNDEFINED;

	// wait for the shape hue to be read
	while (!dnfcomposerHandler.getHaveFieldsSettled());
}

void ExperimentHandler::readTargetAngle()
{
	// wait for the target angle
	do
		signals.targetAngle = dnfcomposerHandler.getOutputFieldCentroid();
	while (signals.targetAngle == data.lastOutputFieldCentroid);

	data.outputFieldCentroid = signals.targetAngle;
	data.lastOutputFieldCentroid = signals.targetAngle;

	// set the target angle for coppelia
	coppeliasimHandler.setSignals(signals);

	dnfcomposerHandler.getUserInterfaceWindow()->setData(data.shapeHue, data.expectedTargetAngle);
}

void ExperimentHandler::cleanUpTrial()
{
	coppeliasimHandler.resetSignals();
}

void ExperimentHandler::updateStatistics()
{
	stats.numDecisions++;

	stats.decisionRatio = (static_cast<double>(stats.numCorrectDecisions) / stats.numDecisions) * 100;

	dnfcomposerHandler.getUserInterfaceWindow()->setStatistics(stats.numDecisions, stats.decisionRatio, stats.numCorrectDecisions);
}

bool ExperimentHandler::verifyDecision()
{
	// Check if cuboidHue exists in the map
	auto closestHueIter = hueToAngleMap.end();
	double minDifference = param.decisionTolerance;

	for (auto it = hueToAngleMap.begin(); it != hueToAngleMap.end(); ++it)
	{
		double difference = std::abs(data.shapeHue - it->first);
		if (difference <= param.decisionTolerance && difference < minDifference)
		{
			minDifference = difference;
			closestHueIter = it;
		}
	}

	if (closestHueIter != hueToAngleMap.end())
	{
		data.expectedTargetAngle = closestHueIter->second;
		dnfcomposerHandler.getUserInterfaceWindow()->setData(data.shapeHue, data.expectedTargetAngle);
		bool isCorrectDecision = std::abs(data.expectedTargetAngle - data.outputFieldCentroid) <= param.decisionTolerance;
		if (isCorrectDecision)
		{
			stats.numCorrectDecisions++; // Increment numCorrectDecisions if the robotTargetAngle is within the decisionTolerance of the target_angle.
			return true;
		}
	}

	//stats.numIncorrectDecisions++; // Increment numIncorrectDecisions if the robotTargetAngle is not within the decisionTolerance of the target_angle.
	// No matching rules for the given cuboidHue and robotTargetAngle.
	return false;
}

void ExperimentHandler::relearningProcedure()
{
	static bool isCorrectDecision = false;
	do {
		// re-train
		// give stimulus again
		readTargetAngle();
		isCorrectDecision = verifyDecision();
		stats.numOfRelearningCycles++;
	} while (!isCorrectDecision);
}

void ExperimentHandler::degenerationProcedure()
{
	int size = 0;
	switch (param.degeneracyType)
	{
	case ElementDegeneracyType::NEURONS_DEACTIVATE:
		size = 360; // completely hardcoded
		break;
	case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
	case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
	case ElementDegeneracyType::WEIGHTS_REDUCE: 
		size = 360 * 180; // completely hardcoded
		break;
	default:
		return;
	}
	int numberOfElementsToDegenerate = param.percentageOfDegeneration * size / 100;
	//std::cout << "numberOfElementsToDegenerate: " << numberOfElementsToDegenerate << std::endl;
	for (int i = 0; i < numberOfElementsToDegenerate; i++)
		dnfcomposerHandler.setDegeneracy(param.degeneracyType);
}
