
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
	// Perform a small demonstration of the working architecture
	//pickAndPlace();

	// For an initial amount of degeneration
	//for (int i = 0; i < param.initialPercentageOfDegeneration/10; i++)
	//	degenerationProcedure();
	//param.accumulatedPercentageOfDegeneration = param.initialPercentageOfDegeneration;

	// For the amount of tenths of percentages we want the sim to run
	//for (int i = 0; i < param.numberOfTenthsOfPercentageToDegenerate; i++)
	//{
		//param.accumulatedPercentageOfDegeneration = param.accumulatedPercentageOfDegeneration + param.percentageOfDegeneration;
		//degenerationProcedure();
		for (int i = 0; i < param.numberOfTrials; i++)
			pickAndPlaceWithLearning();
	//}
}

void ExperimentHandler::close()
{
	dnfcomposerHandler.close();
	coppeliasimHandler.close();
}

void ExperimentHandler::pickAndPlace()
{
	if (INFO)
		std::cout << "Starting a demonstration procedure." << std::endl;

	for (int i = 0; i < param.numberOfShapesPerTrial; i++)
	{
		createShape();
		readShapeHue();
		readTargetAngle();
		if (!verifyDecision())
		{
			if (INFO)
				std::cout << "Incorrect decision, aborting..." << std::endl;
			return;
		}
		graspShape();
		placeShape();
		updateStatistics();
		cleanUpTrial();
	}
}

void ExperimentHandler::pickAndPlaceWithLearning()
{
	//dnfcomposerHandler.clearRelearning();

	for (int i = 0; i < param.numberOfShapesPerTrial; i++)
	{
		createShape();
		readShapeHue();
		readTargetAngle();
		if (!verifyDecision())
		{
			if (INFO)
				std::cout << "Incorrect decision, relearning procedure started." << std::endl;
			relearningProcedure();
		}
		saveLearningCyclesPerTrial();
		graspShape();
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
	coppeliasimHandler.setSignals(signals);
}

void ExperimentHandler::readShapeHue()
{
	// wait for the hue of the cuboid
	do
	{
		Sleep(50);
		signals.shapeHue = coppeliasimHandler.getSignals().shapeHue;
		if (INFO)
			std::cout << "Shape hue: " << signals.shapeHue << std::endl;
	}
	while (signals.shapeHue == UNDEFINED);

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
	// wait for the target angle
	do
	{
		Sleep(50);
		signals.targetAngle = dnfcomposerHandler.getOutputFieldCentroid();
		if (INFO)
			std::cout << "Target angle: " << signals.targetAngle << std::endl;
	}
	while (signals.targetAngle != -1 && (signals.targetAngle == data.lastOutputFieldCentroid || signals.targetAngle < 0.1));
	// This condition will ensure that the loop continues as long as signals.targetAngle is not equal to -1 
	// and either equals data.lastOutputFieldCentroid or is less than 0.1. If signals.targetAngle becomes -1, the loop will stop.

	data.outputFieldCentroid = signals.targetAngle;
	data.lastOutputFieldCentroid = signals.targetAngle;

	// set the target angle for coppelia
	coppeliasimHandler.setSignals(signals);

	dnfcomposerHandler.getUserInterfaceWindow()->setData(data.shapeHue, data.expectedTargetAngle);
}

void ExperimentHandler::cleanUpTrial()
{
	coppeliasimHandler.resetSignals();
	stats.numDecisions = 0;
	stats.numCorrectDecisions = 0;
	stats.numIncorrectDecisions = 0;
	stats.decisionRatio = 0;
	stats.numOfRelearningCycles = 0;
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

	stats.numIncorrectDecisions++; // Increment numIncorrectDecisions if the robotTargetAngle is not within the decisionTolerance of the target_angle.
	// No matching rules for the given cuboidHue and robotTargetAngle.
	return false;
}

void ExperimentHandler::relearningProcedure()
{
	static bool isCorrectDecision = false;
	do {
		if (INFO)
			std::cout << "Relearning..." << std::endl;
		
		signals.targetAngle = UNDEFINED;
		dnfcomposerHandler.setRelearning(signals.shapeHue, signals.targetAngle);

		while (!dnfcomposerHandler.getHasRelearningFinished());									
		
		readShapeHue();
		readTargetAngle();
		
		isCorrectDecision = verifyDecision();
		
		stats.numOfRelearningCycles++;
		if (INFO)
			std::cout << "Relearning cycle: " << stats.numOfRelearningCycles << std::endl;
	} while (!isCorrectDecision && stats.numOfRelearningCycles < 200);
	// Stop we reach the maximum of relearning cycles = 200 (arbitrary?)
}

void ExperimentHandler::degenerationProcedure()
{
	if (INFO)
		std::cout << "Starting the degeneration procedure." << std::endl;

	// Disable the user interface whilst degenerating to consume less time.
	dnfcomposerHandler.setIsUserInterfaceActiveAs(false);

	int numberOfElementsToDegenerate = computeNumberOfElementsToDegenerate();
	for (int i = 0; i < numberOfElementsToDegenerate; i++)
	{
		dnfcomposerHandler.setDegeneracy(param.degeneracyType);
		Sleep(10);
		if (i % 20 == 0)
			if(INFO)
				std::cout << "Number of elements degenerated: " << i << std::endl;
	}

	// Re-enable the UI.
	dnfcomposerHandler.setIsUserInterfaceActiveAs(true);
}

int ExperimentHandler::computeNumberOfElementsToDegenerate()
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
		return 0;
	}

	return param.percentageOfDegeneration * size / 100;
}

void ExperimentHandler::saveLearningCyclesPerTrial() {
	std::string filename = param.filePathPrefix + param.degeneracyName + "-" + std::to_string(param.accumulatedPercentageOfDegeneration) + ".txt";
	std::ofstream file(filename, std::ios::app); // Open the file in append mode
	if (file.is_open()) {
		file << stats.numOfRelearningCycles << "\n"; // Write the integer followed by a newline
		file.close(); // Close the file
		std::cout << "Number of relearning cycles needed saved to file: " << stats.numOfRelearningCycles << std::endl;
	}
	else {
		std::cerr << "Unable to open file: " << filename << std::endl;
	}
}