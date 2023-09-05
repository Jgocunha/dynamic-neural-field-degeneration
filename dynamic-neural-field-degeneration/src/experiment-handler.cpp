
#include "../include/experiment-handler.h"

ExperimentHandler::ExperimentHandler(const ExperimentParameters& param)
	: dnfcomposerHandler(DnfcomposerHandler(param.isVisualisationOn)), param(param)
{
}

// UI updates
void ExperimentHandler::printExperimentSetupToConsole() const
{
	std::cout << "Starting the experiment." << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Number of shapes per trial: " << param.numberOfShapesPerTrial << std::endl;
	std::cout << "Number of trials: " << param.numberOfTrials << std::endl;
	std::cout << "Decision tolerance: " << param.decisionTolerance << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Degeneracy type: " << param.degeneracyName << std::endl;
	std::cout << "Initial percentage of degeneration: " << param.initialPercentageOfDegeneration << std::endl;
	std::cout << "Target percentage of degeneration: " << param.targetPercentageOfDegeneration << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "----------------------------------------" << std::endl << std::endl;
}

void ExperimentHandler::setExperimentSetupData() const
{
	dnfcomposerHandler.setExperimentSetupData(param.degeneracyName, param.decisionTolerance, param.typeOfElementsDegenerated);
}

void ExperimentHandler::setExpectedFieldBehaviour() const
{
	dnfcomposerHandler.setExpectedFieldBehavior(data.shapeHue, data.expectedTargetAngle);
}

void ExperimentHandler::setExperimentAsEnded()
{
	dnfcomposerHandler.stop();
}

// init step and close methods

void ExperimentHandler::init()
{
	dnfcomposerHandler.init();
	coppeliasimHandler.init();
	experimentThread = std::thread(&ExperimentHandler::step, this);
}

// for debug
//void ExperimentHandler::step()
//{
//	printExperimentSetupToConsole();
//	setExperimentSetupData();
//	//setExpectedFieldBehaviour();
//
//	pickAndPlace();
//	cleanUpTrial();
//	pickAndPlace();
//
//	//do 
//	//{
//	//	param.currentPercentageOfDegeneration = param.currentPercentageOfDegeneration + param.incrementOfDegenerationPercentage;
//	//	degenerationProcedure();
//	//	dnfcomposerHandler.saveWeightsToFile();
//	//	std::string backupOfDegenerateWeightsFile = "per - dec_weights - percentage - " + std::to_string(param.currentPercentageOfDegeneration) + ".txt";
//	//	copyWeightsFile(backupOfDegenerateWeightsFile); // create a backup of the weights file
//	//	Sleep(2);
//	//} while (param.currentPercentageOfDegeneration < param.targetPercentageOfDegeneration);
//
//	//}
//}

void ExperimentHandler::step()
{
	printExperimentSetupToConsole();
	setExperimentSetupData();
	
	// Perform a demonstration of the working architecture
	pickAndPlace();

	// If a percentage of degeneration is specified, perform the degeneration procedure
	if(param.initialPercentageOfDegeneration)
		for (int i = 0; i < param.initialPercentageOfDegeneration/10; i++)
			degenerationProcedure();

	bool successfulPickAndPlace = true;
	// Do until you reach the desired amount of degeneration
	do
	{
		// For the number of specified trials
		for (int i = 0; i < param.numberOfTrials; i++)
		{
			// Run the pick and place and the relearning procedures until the pick and place is successful
			do
			{
				successfulPickAndPlace = pickAndPlace();
				if (!successfulPickAndPlace)
				{
					if(!stats.numOfRelearningCycles)
						copyWeightsFile(); // create a backup of the weights file
					relearningProcedure();
					stats.numOfRelearningCycles++;
				}
			} while (!successfulPickAndPlace && (stats.numOfRelearningCycles < param.maximumAmountOfRelearningCycles));

			if (doesBackupWeightsFileExist())
				deleteBackupAndRenameWeightsFile(); // delete the backup and rename the weights file

			saveLearningCyclesPerTrial();
			cleanUpTrial();
		}
		// Once you have finished the specified number of trials for a given amount of degeneration, degenerate the weights
		// and increase the amount of degeneration
		std::string backupOfDegenerateWeightsFile = "per - dec_weights - percentage - " + std::to_string(param.currentPercentageOfDegeneration) + ".txt";
		copyWeightsFile(backupOfDegenerateWeightsFile); // create a backup of the weights file
			
		param.currentPercentageOfDegeneration = param.currentPercentageOfDegeneration + param.incrementOfDegenerationPercentage;
		degenerationProcedure();

		std::cout << "Degenerated " << param.incrementOfDegenerationPercentage << "% of the weights." 
			<< "The current percentage of degeneration is " << param.currentPercentageOfDegeneration 
			<< "%" << std::endl << "--" << std::endl;

	} while (param.currentPercentageOfDegeneration < param.targetPercentageOfDegeneration);

	setExperimentAsEnded();
}

void ExperimentHandler::close()
{
	dnfcomposerHandler.close();
	coppeliasimHandler.close();
}



bool ExperimentHandler::pickAndPlace()
{
	std::cout << "Executing the pick and place procedure." << std::endl;

	stats.shapesPlacedIncorrectly = 0; // binary representation
	bool successfulPickAndPlace = true;

	for (int i = 0; i < param.numberOfShapesPerTrial; i++)
	{
		createShape();
		readShapeHue();
		readTargetAngle();
		if (!verifyDecision())
		{
			successfulPickAndPlace = false;
			stats.shapesPlacedIncorrectly = stats.shapesPlacedIncorrectly << 1;
		}
		else
			stats.shapesPlacedIncorrectly = (stats.shapesPlacedIncorrectly << 1) | 1;
		graspShape();
		placeShape();
		//updateStatistics();
		coppeliasimHandler.resetSignals();

	}

	std::cout << "Binary representation of placed boxes: " << std::bitset<7>(stats.shapesPlacedIncorrectly) << std::endl;
	std::cout << "Pick and place procedure finished, with " << successfulPickAndPlace << " success." << std::endl;

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

		if (param.isVisualisationOn)
			setExpectedFieldBehaviour();

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
	//do
	{
		//Sleep(50);
		signals.targetAngle = dnfcomposerHandler.getOutputFieldCentroid();
		if (INFO)
			std::cout << "Target angle: " << signals.targetAngle << std::endl;
	}
	if (signals.targetAngle == -1)
	{
		signals.targetAngle = 0;
		if (INFO)
			std::cout << "New target angle: " << signals.targetAngle << std::endl;
	}
	//while (signals.targetAngle != -1 && (signals.targetAngle == data.lastOutputFieldCentroid || signals.targetAngle < 0.1));
	//while (signals.targetAngle == -1);
	// This condition will ensure that the loop continues as long as signals.targetAngle is not equal to -1 
	// and either equals data.lastOutputFieldCentroid or is less than 0.1. If signals.targetAngle becomes -1, the loop will stop.

	data.outputFieldCentroid = signals.targetAngle;
	data.lastOutputFieldCentroid = signals.targetAngle;

	// set the target angle for coppelia
	coppeliasimHandler.setSignals(signals);
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
	//stats.numDecisions++;

	//stats.decisionRatio = (static_cast<double>(stats.numCorrectDecisions) / stats.numDecisions) * 100;

	//dnfcomposerHandler.getUserInterfaceWindow()->setStatistics(stats.numDecisions, stats.decisionRatio, stats.numCorrectDecisions);
}



void ExperimentHandler::relearningProcedure()
{
	std::cout << "Pick and place unsuccessful, starting the relearning procedure." << std::endl;

	static bool isCorrectDecision = false;
	dnfcomposerHandler.setRelearning(signals.shapeHue, signals.targetAngle);
	do {
		//std::cout << "Relearning..." << std::endl;
		signals.targetAngle = UNDEFINED;
		Sleep(200);
	} while (!dnfcomposerHandler.getHasRelearningFinished());
	Sleep(200);
}

// DEGENERATION FUNCTIONS
// ================================================================================
void ExperimentHandler::degenerationProcedure()
{
	std::cout << "Starting the degeneration procedure." << std::endl;

	// Disable the user interface whilst degenerating to consume less time.
	dnfcomposerHandler.setIsUserInterfaceActiveAs(false);

	int numberOfElementsToDegenerate = computeNumberOfElementsToDegenerate();
	std::cout << "Number of elements to degenerate: " << numberOfElementsToDegenerate << std::endl;
	for (int i = 0; i < numberOfElementsToDegenerate; i++)
	{
		dnfcomposerHandler.setDegeneracy(param.degeneracyType, param.fieldToDegenerate);
		Sleep(25);
		//dnfcomposerHandler.clearDegeneration();
		//std::cout << "Degenerating..." << std::endl;
	}

	// Re-enable the UI.
	dnfcomposerHandler.setIsUserInterfaceActiveAs(true);
}

int ExperimentHandler::computeNumberOfElementsToDegenerate() const
{
	int size = 0;
	switch (param.degeneracyType)
	{
	case ElementDegeneracyType::NEURONS_DEACTIVATE:
		size = 360; // completely hardcoded
		return 36;
		break;
	case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
	case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
	case ElementDegeneracyType::WEIGHTS_REDUCE:
		size = 360 * 180; // completely hardcoded
		return 6480;
		break;
	default:
		return 0;
	}

	return param.incrementOfDegenerationPercentage * size / 100;
}
// ================================================================================



// DATA COLLECTION FUNCTIONS
// ================================================================================
void ExperimentHandler::copyWeightsFile(const std::string& newFilename) const
{
	std::string filename = param.filePathPrefix + "per - dec_weights.txt";
	std::string filenameCopy = param.filePathPrefix + newFilename;

	std::ifstream source(filename, std::ios::binary);
	std::ofstream dest(filenameCopy, std::ios::binary);

	dest << source.rdbuf();

	source.close();
	dest.close();
}

void ExperimentHandler::deleteBackupAndRenameWeightsFile() const
{
	std::string oldname = param.filePathPrefix + "per - dec_weights - copy.txt";
	std::string newname = param.filePathPrefix + "per - dec_weights.txt";

	try {
		std::remove(newname.c_str());
	}
	catch (...) {}

	int result = std::rename(oldname.c_str(), newname.c_str());

	if (result == 0)
		puts("File successfully renamed.");
	else
		perror("Error renaming file.");
}

bool ExperimentHandler::doesBackupWeightsFileExist() const 
{
	std::string filename = param.filePathPrefix + "per - dec_weights - copy.txt";
	std::ifstream file(filename);

	if (file.good())
		return true;
	else
		return false;
}

void ExperimentHandler::saveLearningCyclesPerTrial() const
{
	std::string filename = param.filePathPrefix + param.degeneracyName + "-" + std::to_string(param.currentPercentageOfDegeneration) + ".txt";
	std::ofstream file(filename, std::ios::app); // Open the file in append mode
	if (file.is_open()) 
	{
		file << " - " << stats.numOfRelearningCycles << "\n"; // Write the integer followed by a newline
		file.close(); // Close the file
		std::cout << "Number of relearning cycles needed saved to file: " << stats.numOfRelearningCycles << std::endl;
	}
	else 
	{
		std::cerr << "Unable to open file: " << filename << std::endl;
	}
}

void ExperimentHandler::saveNumberOfIncorrectlyPlacedBoxes() const
{
	std::string filename = param.filePathPrefix + param.degeneracyName + "-" + std::to_string(param.currentPercentageOfDegeneration) + ".txt";
	std::ofstream file(filename, std::ios::app); // Open the file in append mode
	if (file.is_open())
	{
		file << stats.shapesPlacedIncorrectly << " "; // Write the integer followed by a newline
		file.close(); // Close the file
		std::cout << "Number of shapes placed incorrectly saved to file: " << stats.numOfRelearningCycles << std::endl;
	}
	else
	{
		std::cerr << "Unable to open file: " << filename << std::endl;
	}
}

// ================================================================================
