
#include "../include/experiment-handler.h"

ExperimentHandler::ExperimentHandler(const ExperimentParameters& params)
	: dnfcomposerHandler(DnfcomposerHandler(params.isComposerVisualizationOn)), params(params)
{
	this->params.setOtherDegeneracyParameters();
	dnfcomposerHandler.setExperimentSetupData(params.degeneracyName, params.decisionTolerance, params.typeOfElementsDegenerated);
	dnfcomposerHandler.setRelearningParameters(params.relearningType, params.numberOfRelearningEpochs, params.learningRate, 
		params.maximumAmountOfDemonstrations, params.updateAllWeights);

}

void ExperimentHandler::printExperimentParameters() const
{
	std::cout << "Experiment identifier: " << params.experimentId << std::endl;
	std::cout << "----------------------------------------" << std::endl;
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
	std::cout << "Maximum allowed amount of relearning cycles: " << params.maximumAmountOfDemonstrations << std::endl;
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
	params.experimentId = "trials-" + std::to_string(params.numberOfTrials)
		+ " deg. type-" + std::to_string(static_cast<int>(params.degeneracyType))
		+ " target field-" + params.fieldToDegenerate
		+ " initial per.-" + std::to_string(params.initialPercentageOfDegeneration)
		+ " inc per.-" + std::to_string(params.incrementOfDegenerationPercentage)
		+ " epochs-" + std::to_string(params.numberOfRelearningEpochs)
		+ " demos-" + std::to_string(params.maximumAmountOfDemonstrations);

	printExperimentParameters();
	createExperimentFolderDirectory();
	getOriginalWeightsFile();
	dnfcomposerHandler.init();
	if(params.isLinkToCoppeliaSimOn)
		coppeliasimHandler.init();
	experimentThread = std::thread(&ExperimentHandler::step, this);
}

void ExperimentHandler::step()
{
	

	for(int trial = 1; trial <= params.numberOfTrials; trial++)
	{
		if (params.isDebugModeOn)
		{
			std::cout << std::endl << "Trial " << trial << std::endl;
			std::cout << "----------------------------------------" << std::endl;
		}

		if(params.isComposerVisualizationOn)
			dnfcomposerHandler.setTrial(trial);

		if (params.initialPercentageOfDegeneration != 0)
		{
			mockPickAndPlace();
			while (params.currentPercentageOfDegeneration < params.initialPercentageOfDegeneration)
			{
				params.currentPercentageOfDegeneration += params.incrementOfDegenerationPercentage;
				degenerationProcedure();
			}
			dnfcomposerHandler.saveWeightsToFile();
			if (params.isDebugModeOn)
				std::cout << "Degenerated to " << params.currentPercentageOfDegeneration << "%." << std::endl;
		}

		do
		{
			bool successfulPickAndPlace = false;

			if(params.isLinkToCoppeliaSimOn)
				successfulPickAndPlace = bonafidePickAndPlace();
			else
				successfulPickAndPlace = mockPickAndPlace();

			if (successfulPickAndPlace || (stats.numOfRelearningCycles >= params.maximumAmountOfDemonstrations))
			{
				if (doesBackupWeightsFileExist())
				{
					restoreWeightsFile();
					dnfcomposerHandler.readWeights();
					Sleep(50);
					dnfcomposerHandler.saveWeightsToFile();
					Sleep(50);
				}

				degenerationProcedure();
				
				dnfcomposerHandler.saveWeightsToFile();
				if (stats.numOfRelearningCycles >= params.maximumAmountOfDemonstrations)
				{
					data.isFieldDead = true;
					if (params.isDebugModeOn)
						std::cout << "Field is 'dead'." << std::endl << std::endl;
				}
				stats.learningCyclesPerTrialHistory.push_back(stats.numOfRelearningCycles);
				stats.numOfRelearningCycles = 0;
				params.currentPercentageOfDegeneration += params.incrementOfDegenerationPercentage;
				if (params.isDebugModeOn)
					std::cout << "Degenerated to " << params.currentPercentageOfDegeneration << "%." << std::endl << std::endl;

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
	deleteExperimentFolderDirectory();
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
	//std::cout << "Wrong decision: " << data.expectedTargetAngle << std::endl;
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
	Sleep(10);
	signals.targetAngle = dnfcomposerHandler.getOutputFieldCentroid();
	//if (params.isDebugModeOn)
		//std::cout << "Target angle: " << signals.targetAngle << std::endl;

	data.outputFieldCentroid = signals.targetAngle;
	data.lastOutputFieldCentroid = signals.targetAngle;

	getExpectedTargetAngle();
	dnfcomposerHandler.updateFieldCentroids();

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
	if (params.isComposerVisualizationOn)
		dnfcomposerHandler.setIsUserInterfaceActiveAs(false);


	//int numberOfElementsToDegenerate = computeNumberOfElementsToDegenerate();
	// we kill 1 neuron per iteration
	// we kill 10 weights per iteration
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
	if (params.isComposerVisualizationOn)
		dnfcomposerHandler.setIsUserInterfaceActiveAs(true);
}

int ExperimentHandler::getNumberOfElementsToDegenerate() const
{
	switch (params.degeneracyType)
	{
	case ElementDegeneracyType::NEURONS_DEACTIVATE:
		if(params.fieldToDegenerate == "perceptual")
			return 1; // 1 element - 0.36%
		if (params.fieldToDegenerate == "decision")
			return 1; // 1 element - 3.6%
	case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
	case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
	case ElementDegeneracyType::WEIGHTS_REDUCE:
		return 25; // 250 elements - 2.48%
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

	if (params.isDebugModeOn)
		std::cout << "Relearning procedure finished." << std::endl << std::endl;

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
	Sleep(10);
	dnfcomposerHandler.setWasCloseSimulationRequested(true);
	Sleep(10);
	dnfcomposerHandler.setWasStartSimulationRequested(true);
	//Sleep(25);
}

void ExperimentHandler::saveLearningCyclesPerTrial() const
{
	//const std::string filename = 
		//params.filePathPrefix + "results/" + params.degeneracyName + ".txt";
	const std::string filename = params.getSavePath();

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
	std::string filename = params.filePathPrefix + params.experimentId + "/weights/" + "per - dec_weights.txt";
	std::string filenameCopy = params.filePathPrefix + params.experimentId + "/weights/" + newFilename;

	std::ifstream source(filename, std::ios::binary);
	std::ofstream dest(filenameCopy, std::ios::binary);

	dest << source.rdbuf();

	std::cout << "Backing up weights file." << std::endl;

	source.close();
	dest.close();
}

void ExperimentHandler::restoreWeightsFile() const
{
	const std::string oldFilename = params.filePathPrefix + params.experimentId + "/weights/" + "per - dec_weights - copy.txt";
	const std::string newFilename = params.filePathPrefix + params.experimentId + "/weights/" + "per - dec_weights.txt";

	int result = std::remove(newFilename.c_str());

	if (!result)
		std::cout << "Previous weights file successfully deleted." << std::endl;
	else
		std::cout << "Error deleting previous weights file." << std::endl;

	const std::string newTestFilename = params.filePathPrefix + params.experimentId + "/weights/" + "per - dec_weights.txt";

	result = std::rename(oldFilename.c_str(), newTestFilename.c_str());

	if (!result)
		std::cout << "File successfully renamed." << std::endl;
	else
		std::cout << "Error renaming file." << std::endl;
}

bool ExperimentHandler::doesBackupWeightsFileExist() const
{
	const std::string filename = params.filePathPrefix + params.experimentId + "/weights/" + "per - dec_weights - copy.txt";
	const std::ifstream file(filename);

	if (file.good())
	{
		std::cout << "Weights file exists." << std::endl;
		return true;
	}

	std::cout << "Weights file does not exist." << std::endl;
	return false;
}

void ExperimentHandler::getOriginalWeightsFile() const
{
	const std::string sourceFileName = params.filePathPrefix + "weights-backup/per - dec_weights.txt";
	const std::ifstream sourceFile(sourceFileName);

	const std::string destFileName = params.filePathPrefix + params.experimentId + "/weights/" + "per - dec_weights.txt";
	std::ofstream destFile(destFileName);

	destFile << sourceFile.rdbuf();
}

void ExperimentHandler::createExperimentFolderDirectory() 
{
	namespace fs = std::filesystem;

	const std::string experimentFolderPath = params.filePathPrefix + params.experimentId;
	fs::create_directory(experimentFolderPath);

	const std::string weightsFolderPath = params.filePathPrefix + params.experimentId + "/weights";
	fs::create_directory(weightsFolderPath);

	dnfcomposerHandler.setDataFilePath(weightsFolderPath);

}

void ExperimentHandler::deleteExperimentFolderDirectory() const
{
	namespace fs = std::filesystem;

	const std::string experimentFolderPath = params.filePathPrefix + params.experimentId;
	fs::remove_all(experimentFolderPath);
}
