
#include "../include/experiment-handler.h"

ExperimentHandler::ExperimentHandler(const ExperimentParameters& params)
	: dnfcomposerHandler(DnfcomposerHandler(params.isComposerVisualizationOn)), params(params)
{
	this->params.setOtherDegeneracyParameters();
	printExperimentParameters();
	dnfcomposerHandler.setExperimentSetupData(params.degeneracyName, params.decisionTolerance, params.typeOfElementsDegenerated);
	dnfcomposerHandler.setRelearningParameters(params.relearningType, params.numberOfRelearningEpochs, params.learningRate, 
		params.maximumAmountOfRelearningCycles, params.updateAllWeights);

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
	std::cout << "Relearning type: " << (params.relearningType == RelearningParameters::RelearningType::ALL_CASES ? "All cases" : "Only degenerated cases") << std::endl;
	std::cout << "Learning rate: " << params.learningRate << std::endl;
	std::cout << "Number of relearning epochs: " << params.numberOfRelearningEpochs << std::endl;
	std::cout << "Maximum allowed amount of relearning cycles: " << params.maximumAmountOfRelearningCycles << std::endl;
	std::cout << "Update all weights: " << (params.updateAllWeights ? "true" : "false") << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Is data saving on: " << (params.isDataSavingOn ? "true" : "false") << std::endl;
	std::cout << "Is dnf-composer visualization on: " << (params.isComposerVisualizationOn ? "true" : "false") << std::endl;
	std::cout << "Is debug mode on: " << (params.isDebugModeOn ? "true" : "false") << std::endl;
	std::cout << "Is link to CoppeliaSim on: " << (params.isLinkToCoppeliaSimOn ? "true" : "false") << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "----------------------------------------" << std::endl << std::endl;
}

void ExperimentHandler::init()
{
	getOriginalWeightsFile();
	dnfcomposerHandler.init();
	if(params.isLinkToCoppeliaSimOn)
		coppeliasimHandler.init();
	experimentThread = std::thread(&ExperimentHandler::step, this);
}


void ExperimentHandler::step()
{
	if(params.initialPercentageOfDegeneration != 0)
	{
		mockPickAndPlace();
		while(params.currentPercentageOfDegeneration <= params.initialPercentageOfDegeneration)
		{
			params.currentPercentageOfDegeneration += params.incrementOfDegenerationPercentage;
			degenerationProcedure();
			dnfcomposerHandler.saveWeightsToFile();
		}
		if(params.isDebugModeOn)
			std::cout << "Degenerated to " << params.currentPercentageOfDegeneration << "%." << std::endl;
	}

	for(int trial = 1; trial <= params.numberOfTrials; trial++)
	{
		if (params.isDebugModeOn)
		{
			std::cout << std::endl << "Trial " << trial << std::endl;
			std::cout << "----------------------------------------" << std::endl;
		}

		if(params.isComposerVisualizationOn)
			dnfcomposerHandler.setTrial(trial);

		do
		{
			bool successfulPickAndPlace = false;

			if(params.isLinkToCoppeliaSimOn)
				successfulPickAndPlace = bonafidePickAndPlace();
			else
				successfulPickAndPlace = mockPickAndPlace();

			if (successfulPickAndPlace || (stats.numOfRelearningCycles >= params.maximumAmountOfRelearningCycles))
			{
				if (doesBackupWeightsFileExist())
					restoreWeightsFile();
				degenerationProcedure();
				dnfcomposerHandler.saveWeightsToFile();
				if (stats.numOfRelearningCycles >= params.maximumAmountOfRelearningCycles)
					data.isFieldDead = true;
				stats.learningCyclesPerTrialHistory.push_back(stats.numOfRelearningCycles);
				stats.numOfRelearningCycles = 0;
				params.currentPercentageOfDegeneration += params.incrementOfDegenerationPercentage;
			}
			else
			{
				if(!doesBackupWeightsFileExist())
					backupWeightsFile();
				relearningProcedure();
			}
			if (params.isComposerVisualizationOn)
				dnfcomposerHandler.setRelearningCycles(stats.numOfRelearningCycles);
			cleanupPickAndPlace();

		} while ((params.currentPercentageOfDegeneration < params.targetPercentageOfDegeneration) && !(data.isFieldDead));

		cleanupTrial();
	}
	dnfcomposerHandler.stop();
}

//void ExperimentHandler::step()
//{
//	//mockPickAndPlace();
//
//	//while(params.currentPercentageOfDegeneration <= params.initialPercentageOfDegeneration)
//	//{
//	//	params.currentPercentageOfDegeneration += params.incrementOfDegenerationPercentage;
//	//	degenerationProcedure();
//	//	std::cout << "Degeneration percentage: " << params.currentPercentageOfDegeneration << std::endl;
//	//	Sleep(10);
//	//	dnfcomposerHandler.saveWeightsToFile();
//	//}
//
//	//Sleep(200);
//
//	for(int i = 0; i < params.numberOfTrials; i++)
//	{
//		if(params.isDebugModeOn)
//		{
//			std::cout << "Trial " << i + 1 << " started." << std::endl;
//			std::cout << "----------------------------------------" << std::endl;
//		}
//
//		do
//		{
//			bool successfulPickAndPlace = false;
//			do
//			{
//				if(params.isLinkToCoppeliaSimOn)
//					successfulPickAndPlace = bonafidePickAndPlace();
//				else
//					successfulPickAndPlace = mockPickAndPlace();
//
//				if(!successfulPickAndPlace)
//				{
//					if (!doesBackupWeightsFileExist())
//						backupWeightsFile();
//					relearningProcedure();
//				}
//			} while (!successfulPickAndPlace 
//				&& (stats.numOfRelearningCycles < params.maximumAmountOfRelearningCycles));
//
//			saveLearningCyclesPerTrial();
//
//			restoreWeightsFile();
//			degenerationProcedure();
//			dnfcomposerHandler.saveWeightsToFile();
//			params.currentPercentageOfDegeneration += params.incrementOfDegenerationPercentage;
//			
//		} while (params.currentPercentageOfDegeneration <= params.targetPercentageOfDegeneration );
//
//		cleanupTrial();
//	}
//
//}

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
		//if (params.isDebugModeOn)
			//std::cout << "Shape hue: " << signals.shapeHue << std::endl;
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
	dnfcomposerHandler.updateFieldCentroids();
	Sleep(5);
	signals.targetAngle = dnfcomposerHandler.getOutputFieldCentroid();
	//if (params.isDebugModeOn)
		//std::cout << "Target angle: " << signals.targetAngle << std::endl;

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
	//if (params.isDebugModeOn)
		//std::cout << "External stimulus: " << data.shapeHue << std::endl;
	++hueToAngleIterator;

	dnfcomposerHandler.setExternalInput(data.shapeHue);

	// wait for the shape hue to be read
	while (!dnfcomposerHandler.getHaveFieldsSettled());
	dnfcomposerHandler.setHaveFieldsSettled(false);
}

void ExperimentHandler::mockReadTargetAngle()
{
	dnfcomposerHandler.updateFieldCentroids();
	Sleep(5);
	signals.targetAngle = dnfcomposerHandler.getOutputFieldCentroid();
	//if (params.isDebugModeOn)
		//std::cout << "Target angle: " << signals.targetAngle << std::endl;

	data.outputFieldCentroid = signals.targetAngle;
	data.lastOutputFieldCentroid = signals.targetAngle;

	getExpectedTargetAngle();
	dnfcomposerHandler.updateFieldCentroids();
}

void ExperimentHandler::degenerationProcedure()
{
	if(params.isDebugModeOn)
		std::cout << "Degeneration procedure started." << std::endl;

	// Disable the user interface whilst degenerating to consume less time.
	dnfcomposerHandler.setIsUserInterfaceActiveAs(false);

	//int numberOfElementsToDegenerate = computeNumberOfElementsToDegenerate();
	// we kill 1 neuron per iteration
	// we kill 100 weights per iteration
	const int numberOfElementsToDegenerate = getNumberOfElementsToDegenerate();

	if (params.isDebugModeOn)
		std::cout << "Number of elements to degenerate: " << numberOfElementsToDegenerate << std::endl;

	for (int i = 0; i < numberOfElementsToDegenerate; i++)
	{
		dnfcomposerHandler.setDegeneracy(params.degeneracyType, params.fieldToDegenerate);
		while (!dnfcomposerHandler.getHaveFieldsSettled());
		dnfcomposerHandler.setHaveFieldsSettled(false);
	}

	// Re-enable the UI.
	dnfcomposerHandler.setIsUserInterfaceActiveAs(true);
}

int ExperimentHandler::getNumberOfElementsToDegenerate() const
{
	switch (params.degeneracyType)
	{
	case ElementDegeneracyType::NEURONS_DEACTIVATE:
		if(params.fieldToDegenerate == "perceptual")
			return 36; 
		if (params.fieldToDegenerate == "decision")
			return 18; 
	case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
	case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
	case ElementDegeneracyType::WEIGHTS_REDUCE:
		return 65; //64.8
	default:
		return 0;
	}
}

void ExperimentHandler::relearningProcedure()
{
	// make sure to test two alternatives
	// 1. use the 7 inputs
	// 2. use only the inputs from the incorrect correspondence
	// Here we can also test running for 1 iteration vs. 100 iterations per learning cycle
	// And the learning rate

	if(params.isDebugModeOn)
		std::cout << "Relearning procedure started." << std::endl;

	dnfcomposerHandler.setRelearning(stats.shapesPlacedIncorrectly);

	while (!dnfcomposerHandler.getHasRelearningFinished());
	dnfcomposerHandler.setHasRelearningFinished(false);
	//Sleep(2000);

	stats.numOfRelearningCycles++;
}

void ExperimentHandler::cleanupPickAndPlace()
{
	stats.shapesPlacedIncorrectly = 0;
	if(params.isLinkToCoppeliaSimOn)
		coppeliasimHandler.resetSignals();
}

void ExperimentHandler::cleanupTrial()
{
	if(params.isDataSavingOn)
		saveLearningCyclesPerTrial();
	stats.learningCyclesPerTrialHistory.clear();
	params.currentPercentageOfDegeneration = 0;
	data.isFieldDead = false;
	getOriginalWeightsFile();
	Sleep(50);
	dnfcomposerHandler.setWasCloseSimulationRequested(true);
	Sleep(50);
}

void ExperimentHandler::saveLearningCyclesPerTrial() const
{
	const std::string filename = 
		params.filePathPrefix + "results/" + params.degeneracyName + ".txt";
	std::ofstream file(filename, std::ios::app); // Open the file in append mode
	if (file.is_open())
	{
		for (const int cycles : stats.learningCyclesPerTrialHistory) 
			file << cycles << " "; // Write the integer followed by a newline
		file << "\n";
		file.close(); // Close the file
		std::cout << "Number of relearning cycles needed saved to file." << std::endl;
	}
	else
	{
		std::cerr << "Unable to open file: " << filename << std::endl;
	}
}

void ExperimentHandler::backupWeightsFile() const
{
	const std::string newFilename = "per - dec_weights - copy.txt";
	std::string filename = params.filePathPrefix + "per - dec_weights.txt";
	std::string filenameCopy = params.filePathPrefix + newFilename;

	std::ifstream source(filename, std::ios::binary);
	std::ofstream dest(filenameCopy, std::ios::binary);

	dest << source.rdbuf();

	//puts("Backing up weights file.");

	source.close();
	dest.close();
}

void ExperimentHandler::restoreWeightsFile() const
{
	const std::string oldFilename = params.filePathPrefix + "per - dec_weights - copy.txt";
	const std::string newFilename = params.filePathPrefix + "per - dec_weights.txt";

	int result = std::remove(newFilename.c_str());

	//if (!result)
		//puts("Previous weights file successfully deleted.");
	//else
		//perror("Error deleting previous weights file.");

	result = std::rename(oldFilename.c_str(), newFilename.c_str());

	//if (!result)
		//puts("File successfully renamed.");
	//else
		//perror("Error renaming file.");
}

bool ExperimentHandler::doesBackupWeightsFileExist() const
{
	const std::string filename = params.filePathPrefix + "per - dec_weights - copy.txt";
	const std::ifstream file(filename);

	if (file.good())
	{
		//puts("Weights file exists.");
		return true;
	}

	//puts("Weights file does not exist.");
	return false;
}

void ExperimentHandler::getOriginalWeightsFile() const
{
	const std::string sourceFileName = params.filePathPrefix + "weights-backup/weights-original/per - dec_weights.txt";
	const std::ifstream sourceFile(sourceFileName);

	const std::string destFileName = params.filePathPrefix + "per - dec_weights.txt";
	std::ofstream destFile(destFileName);

	destFile << sourceFile.rdbuf();
}
