
#include "../include/experiment-handler.h"

ExperimentHandler::ExperimentHandler(const ExperimentParameters& params)
	: dnfcomposerHandler(DnfcomposerHandler(params.isComposerVisualizationOn)), params(params)
{
	log(dnf_composer::DEBUG, "ExperimentHandler::ExperimentHandler()\n");
	this->params.setOtherDegeneracyParameters();
	dnfcomposerHandler.setExperimentSetupData(this->params.degeneracyName, this->params.decisionTolerance, this->params.typeOfElementsDegenerated);
	dnfcomposerHandler.setRelearningParameters(this->params.relearningType, this->params.numberOfRelearningEpochs, this->params.learningRate,
		this->params.maximumAmountOfDemonstrations, this->params.updateAllWeights);
	dnfcomposerHandler.setIncrementOfDegenerationPercentage(this->params.incrementOfDegenerationPercentage);
}

void ExperimentHandler::printExperimentParameters() const
{
	log(dnf_composer::DEBUG, "ExperimentHandler::printExperimentParameters()\n");

	std::ostringstream logStream;

	logStream << "Experiment identifier: " << params.experimentId << std::endl;
	logStream << "----------------------------------------" << std::endl;
	logStream << "Experiment parameters" << std::endl;
	logStream << "----------------------------------------" << std::endl;
	logStream << "Number of shapes per trial: " << params.numberOfShapesPerTrial << std::endl;
	logStream << "Decision tolerance: " << params.decisionTolerance << std::endl;
	logStream << "Number of trials: " << params.numberOfTrials << std::endl;
	logStream << "----------------------------------------" << std::endl;
	logStream << "Degeneracy type: " << params.degeneracyName << std::endl;
	logStream << "Type of elements degenerated: " << params.typeOfElementsDegenerated << std::endl;
	logStream << "----------------------------------------" << std::endl;
	logStream << "Initial percentage of degeneration: " << params.initialPercentageOfDegeneration << std::endl;
	logStream << "Target percentage of degeneration: " << params.targetPercentageOfDegeneration << std::endl;
	logStream << "Increment of degeneration percentage: " << params.incrementOfDegenerationPercentage << std::endl;
	logStream << "----------------------------------------" << std::endl;
	logStream << "Relearning type: " << (params.relearningType == RelearningParameters::RelearningType::ALL_CASES ? "All cases" : "Only degenerated cases") << std::endl;
	logStream << "Learning rate: " << params.learningRate << std::endl;
	logStream << "Number of relearning epochs: " << params.numberOfRelearningEpochs << std::endl;
	logStream << "Maximum allowed amount of relearning cycles: " << params.maximumAmountOfDemonstrations << std::endl;
	logStream << "Update all weights: " << (params.updateAllWeights ? "true" : "false") << std::endl;
	logStream << "----------------------------------------" << std::endl;
	logStream << "Is data saving on: " << (params.isDataSavingOn ? "true" : "false") << std::endl;
	logStream << "Is dnf-composer visualization on: " << (params.isComposerVisualizationOn ? "true" : "false") << std::endl;
	logStream << "Is debug mode on: " << (params.isDebugModeOn ? "true" : "false") << std::endl;
	logStream << "Is link to CoppeliaSim on: " << (params.isLinkToCoppeliaSimOn ? "true" : "false") << std::endl;
	logStream << "----------------------------------------" << std::endl;
	logStream << "----------------------------------------" << std::endl << std::endl;

	log(dnf_composer::LogLevel::INFO, logStream.str());
}

void ExperimentHandler::init()
{
	log(dnf_composer::DEBUG, "ExperimentHandler::init()\n");

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

void ExperimentHandler::initialDegeneration()
{
	log(dnf_composer::DEBUG, "ExperimentHandler::initialDegeneration()\n");

	log(dnf_composer::WARNING, "Number of degenerated elements before pick and place: " + std::to_string(dnfcomposerHandler.getNumberOfDegeneratedElements()) + "\n");

	mockPickAndPlace();

	int numberElements = 0;
	switch (params.degeneracyType)
	{
	case dnf_composer::element::ElementDegeneracyType::NEURONS_DEACTIVATE:
		if (params.fieldToDegenerate == "perceptual")
			numberElements = 720 * params.initialPercentageOfDegeneration / 100 ;
		if (params.fieldToDegenerate == "output")
			numberElements = 280 * params.initialPercentageOfDegeneration / 100;
		break;
	case dnf_composer::element::ElementDegeneracyType::WEIGHTS_DEACTIVATE:
	case dnf_composer::element::ElementDegeneracyType::WEIGHTS_RANDOMIZE:
	case dnf_composer::element::ElementDegeneracyType::WEIGHTS_REDUCE:
		numberElements = 280 * 720 * params.initialPercentageOfDegeneration / 100;
		break;
	default:
		break;
	}

	if (params.isDebugModeOn)
		dnf_composer::log(dnf_composer::INFO, "Initial number of elements to degenerate is: " + std::to_string(numberElements) + '\n');

	dnfcomposerHandler.setInitialNumberOfElementsToDegenerate(numberElements);


	log(dnf_composer::WARNING, "Number of degenerated elements after pick and place: " + std::to_string(dnfcomposerHandler.getNumberOfDegeneratedElements()) + "\n");

	//Sleep(100);

	degenerationProcedure();

	//Sleep(500);
	log(dnf_composer::WARNING, "Number of degenerated elements after degeneration: " + std::to_string(dnfcomposerHandler.getNumberOfDegeneratedElements()) + "\n");


	params.currentPercentageOfDegeneration = params.initialPercentageOfDegeneration;
	if (params.isDebugModeOn)
	{
		std::ostringstream logStream;
		logStream << "Degenerated to " << std::fixed << std::setprecision(2) << params.currentPercentageOfDegeneration << "%.\n";
		dnf_composer::log(dnf_composer::INFO, logStream.str());
	}

	saveWeights();

	log(dnf_composer::WARNING, "Number of degenerated elements after saving: " + std::to_string(dnfcomposerHandler.getNumberOfDegeneratedElements()) + "\n");

	dnfcomposerHandler.setNumberOfElementsToDegenerate();
}

void ExperimentHandler::step()
{
	log(dnf_composer::DEBUG, "ExperimentHandler::step()\n");

	for(int trial = 1; trial <= params.numberOfTrials; trial++)
	{
		if (params.isDebugModeOn)
			dnf_composer::log(dnf_composer::ERROR_, "Trial: " + std::to_string(trial) + '\n');

		if(params.isComposerVisualizationOn)
			dnfcomposerHandler.setTrial(trial);

		//Sleep(10);
		//dnfcomposerHandler.setWasStartSimulationRequested(true);
		//Sleep(100);

		if (params.initialPercentageOfDegeneration != 0)
			initialDegeneration();

		log(dnf_composer::WARNING, "Number of degenerated elements after initialDegeneration(): " + std::to_string(dnfcomposerHandler.getNumberOfDegeneratedElements()) + "\n");

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
					readWeights();
					saveWeights();
				}
				
				degenerationProcedure();
				saveWeights();
				log(dnf_composer::WARNING, "Number of degenerated elements after degeneration and saving: " + std::to_string(dnfcomposerHandler.getNumberOfDegeneratedElements()) + "\n");

				if (stats.numOfRelearningCycles >= params.maximumAmountOfDemonstrations)
				{
					data.isFieldDead = true;
					if (params.isDebugModeOn)
						dnf_composer::log(dnf_composer::INFO, "Re-learning did not work.\n");
				}
				stats.learningCyclesPerTrialHistory.push_back(stats.numOfRelearningCycles);
				stats.numOfRelearningCycles = 0;
				params.currentPercentageOfDegeneration += params.incrementOfDegenerationPercentage;
				if (params.isDebugModeOn)
				{
					std::ostringstream logStream;
					logStream << "Degenerated to " << std::fixed << std::setprecision(2) << params.currentPercentageOfDegeneration << "%.\n";
					dnf_composer::log(dnf_composer::INFO, logStream.str());
				}
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
	log(dnf_composer::DEBUG, "ExperimentHandler::close()");

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
		dnf_composer::log(dnf_composer::INFO, "Binary representation of placed boxes: " + std::bitset<7>(stats.shapesPlacedIncorrectly).to_string() + '\n');
		std::ostringstream logStream;
		logStream << "Pick and place procedure finished, with" << (successfulPickAndPlace ? " success." : "out success.") << std::endl;
		dnf_composer::log(dnf_composer::INFO, logStream.str());
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
	log(dnf_composer::DEBUG, "ExperimentHandler::verifyDecision()\n");

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
	log(dnf_composer::DEBUG, "ExperimentHandler::mockPickAndPlace()\n");

	log(dnf_composer::INFO, "Performing a mock pick and place.\n");

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
		dnf_composer::log(dnf_composer::INFO, "Binary representation of placed boxes: " + std::bitset<7>(stats.shapesPlacedIncorrectly).to_string() + '\n');
		std::ostringstream logStream;
		logStream << "Pick and place procedure finished, with" << (successfulPickAndPlace ? " success." : "out success.") << std::endl;
		dnf_composer::log(dnf_composer::INFO, logStream.str());
	}

	return successfulPickAndPlace;
}

void ExperimentHandler::mockReadShapeHue()
{
	log(dnf_composer::DEBUG, "ExperimentHandler::mockReadShapeHue()\n");

	if(hueToAngleIterator == hueToAngleMap.end())
		hueToAngleIterator = hueToAngleMap.begin();

	data.shapeHue = hueToAngleIterator->first;
	data.expectedTargetAngle = hueToAngleIterator->second;
	//if (params.isDebugModeOn)
		//std::cout << "External stimulus: " << data.shapeHue << std::endl;
	++hueToAngleIterator;

	if (params.isComposerVisualizationOn)
		dnfcomposerHandler.setExpectedFieldBehavior(data.shapeHue, data.expectedTargetAngle);

	dnfcomposerHandler.setExternalInput(data.shapeHue);

	// wait for the shape hue to be read
	while (!dnfcomposerHandler.getHaveFieldsSettled());
	dnfcomposerHandler.setHaveFieldsSettled(false);

}

void ExperimentHandler::mockReadTargetAngle()
{
	log(dnf_composer::DEBUG, "ExperimentHandler::mockReadTargetAngle()\n");

	//dnfcomposerHandler.updateFieldCentroids();
	//Sleep(5);
	signals.targetAngle = dnfcomposerHandler.getOutputFieldCentroid();
	//if (params.isDebugModeOn)
		//std::cout << "Target angle: " << signals.targetAngle << std::endl;

	data.outputFieldCentroid = signals.targetAngle;
	data.lastOutputFieldCentroid = signals.targetAngle;

	//getExpectedTargetAngle();
	//dnfcomposerHandler.updateFieldCentroids();
}

void ExperimentHandler::degenerationProcedure()
{
	log(dnf_composer::DEBUG, "ExperimentHandler::degenerationProcedure()\n");

	if (params.isDebugModeOn)
		dnf_composer::log(dnf_composer::INFO, "Degeneration procedure started.\n");

	// Disable the user interface whilst degenerating to consume less time.
	if (params.isComposerVisualizationOn)
		dnfcomposerHandler.setIsUserInterfaceActiveAs(false);

	dnfcomposerHandler.setDegeneracy(params.degeneracyType, params.fieldToDegenerate);
	while (!dnfcomposerHandler.getHaveFieldsSettled());
	dnfcomposerHandler.setHaveFieldsSettled(false);

	// Re-enable the UI.
	if (params.isComposerVisualizationOn)
		dnfcomposerHandler.setIsUserInterfaceActiveAs(true);
}

void ExperimentHandler::relearningProcedure()
{
	// make sure to test two alternatives
	// 1. use the 7 inputs
	// 2. use only the inputs from the incorrect correspondence
	// Here we can also test running for 1 iteration vs. 100 iterations per learning cycle
	// And the learning rate
	log(dnf_composer::DEBUG, "ExperimentHandler::relearningProcedure()\n");

	if (params.isDebugModeOn)
		dnf_composer::log(dnf_composer::INFO, "Relearning procedure started.\n");

	dnfcomposerHandler.setRelearning(stats.shapesPlacedIncorrectly);

	while (!dnfcomposerHandler.getHasRelearningFinished());
	dnfcomposerHandler.setHasRelearningFinished(false);

	if (params.isDebugModeOn)
		dnf_composer::log(dnf_composer::INFO, "Relearning procedure finished.\n");

	stats.numOfRelearningCycles++;
}

void ExperimentHandler::cleanupPickAndPlace()
{
	log(dnf_composer::DEBUG, "ExperimentHandler::cleanupPickAndPlace()\n");

	stats.shapesPlacedIncorrectly = 0;
	if(params.isLinkToCoppeliaSimOn)
		coppeliasimHandler.resetSignals();
	log(dnf_composer::LogLevel::INFO, "Pick and place procedure finished.\n");
}

void ExperimentHandler::cleanupTrial()
{
	log(dnf_composer::DEBUG, "ExperimentHandler::cleanupTrial()\n");

	if(params.isDataSavingOn)
		saveLearningCyclesPerTrial();
	stats.learningCyclesPerTrialHistory.clear();
	params.currentPercentageOfDegeneration = 0;
	data.isFieldDead = false;
	getOriginalWeightsFile();
	dnfcomposerHandler.setWasCloseSimulationRequested(true);
	Sleep(300);
	//Sleep(300);
	//dnfcomposerHandler.setWasStartSimulationRequested(true);
	log(dnf_composer::LogLevel::INFO, "Trial finished.\n");
}

void ExperimentHandler::saveLearningCyclesPerTrial() const
{
	log(dnf_composer::DEBUG, "ExperimentHandler::saveLearningCyclesPerTrial()\n");

	const std::string filename = params.getSavePath();

	std::ofstream file(filename, std::ios::app); // Open the file in append mode

	if (file.is_open()) {
		for (const int cycles : stats.learningCyclesPerTrialHistory)
			file << cycles << " "; // Write the integer followed by a space

		file << "\n";

		// Flush the data to the file
		file.flush();

		// Check if the stream is in a good state after flushing
		if (file.good()) {
			file.close(); // Close the file
			dnf_composer::log(dnf_composer::INFO, "Number of relearning cycles needed saved to file.\n");
		}
		else {
			dnf_composer::log(dnf_composer::ERROR_, "Error while writing to file: " + filename + '\n');
		}
	}
	else {
		dnf_composer::log(dnf_composer::ERROR_, "Unable to open file: " + filename + '\n');
	}
}

void ExperimentHandler::backupWeightsFile() const
{
	log(dnf_composer::DEBUG, "ExperimentHandler::backupWeightsFile()\n");

	const std::string newFilename = "per - out_weights - copy.txt";
	std::string filename = params.filePathPrefix + params.experimentId + "/weights/" + "per - out_weights.txt";
	std::string filenameCopy = params.filePathPrefix + params.experimentId + "/weights/" + newFilename;

	std::ifstream source(filename, std::ios::binary);
	std::ofstream dest(filenameCopy, std::ios::binary);

	dnf_composer::log(dnf_composer::INFO, "Attempting to back up weights file...\n");

	dest << source.rdbuf();

	source.close();
	dest.close();

	// Check the file sizes
	std::streampos sourceSize, destSize;
	std::ifstream sourceCheckFile(filename, std::ios::binary | std::ios::ate);
	std::ifstream destCheckFile(filenameCopy, std::ios::binary | std::ios::ate);

	do
	{
		sourceSize = sourceCheckFile.tellg();
		destSize = destCheckFile.tellg();
		//log(dnf_composer::DEBUG, "Checking if file size is correct... \n");
		//log(dnf_composer::DEBUG, "Source file size is: " + to_string(sourceSize) + " bytes. Destination file size is: " + to_string(destSize) + " bytes.\n");
	} while (sourceSize != destSize);

	dnf_composer::log(dnf_composer::INFO, "Weights file successfully backed-up.\n");

}

void ExperimentHandler::restoreWeightsFile() const
{
	log(dnf_composer::DEBUG, "ExperimentHandler::restoreWeightsFile()\n");

	const std::string oldFilename = params.filePathPrefix + params.experimentId + "/weights/" + "per - out_weights - copy.txt";
	const std::string newFilename = params.filePathPrefix + params.experimentId + "/weights/" + "per - out_weights.txt";

	//log(dnf_composer::DEBUG, "Attempting to restore weights file...\n");

	// remove weights file
	const int removeResult = std::remove(newFilename.c_str());
	std::ifstream oldFile(newFilename.c_str());
	do
	{
		//(dnf_composer::DEBUG, "Checking if weights file still exists...\n");
		oldFile.open(newFilename.c_str());
	} while (oldFile.is_open());
	oldFile.close();
	//dnf_composer::log(dnf_composer::INFO, "Previous weights file successfully deleted.\n");

	// rename copy of weights file
	std::ifstream currentFile(oldFilename.c_str(), std::ios::binary | std::ios::ate);
	std::streampos sourceSize, destSize;
	sourceSize = currentFile.tellg();

	currentFile.close();
	const int renameResult = std::rename(oldFilename.c_str(), newFilename.c_str());

	dnf_composer::log(dnf_composer::INFO, "File successfully renamed.\n");

	std::ifstream newFile(newFilename.c_str(), std::ios::binary | std::ios::ate);
	do
	{
		destSize = newFile.tellg();
		//log(dnf_composer::DEBUG, "Checking if file size is correct... \n");
		//log(dnf_composer::DEBUG, "Source file size is: " + to_string(sourceSize) + " bytes. Destination file size is: " + to_string(destSize) + " bytes.\n");
	} while (sourceSize != destSize);

	dnf_composer::log(dnf_composer::INFO, "Weights file successfully restored.\n");
}

bool ExperimentHandler::doesBackupWeightsFileExist() const
{
	log(dnf_composer::DEBUG, "ExperimentHandler::doesBackupWeightsFileExist()\n");

	const std::string filename = params.filePathPrefix + params.experimentId + "/weights/" + "per - out_weights - copy.txt";
	const std::ifstream file(filename);

	if (file.good())
	{
		dnf_composer::log(dnf_composer::INFO, "Weights file exists.\n");
		return true;
	}

	dnf_composer::log(dnf_composer::INFO, "Weights file does not exist.\n");
	return false;
}

void ExperimentHandler::getOriginalWeightsFile() const
{
	log(dnf_composer::DEBUG, "ExperimentHandler::getOriginalWeightsFile()\n");

	const std::string sourceFileName = params.filePathPrefix + "weights-backup/per - out_weights.txt";
	 std::ifstream sourceFile(sourceFileName);

	// log(dnf_composer::DEBUG, "Attempting to get original weights file...\n");

	if (!sourceFile.is_open()) 
	{
		dnf_composer::log(dnf_composer::ERROR_, "Failed to open source file: " + sourceFileName);
		return; // Exit the function if the source file cannot be opened
	}

	const std::string destFileName = params.filePathPrefix + params.experimentId + "/weights/" + "per - out_weights.txt";
	std::ofstream destFile(destFileName);

	if (!destFile.is_open()) 
	{
		dnf_composer::log(dnf_composer::ERROR_, "Failed to open destination file: " + destFileName);
		sourceFile.close(); // Close the source file before returning
		return; // Exit the function if the destination file cannot be opened
	}

	destFile << sourceFile.rdbuf();

	if (destFile.fail()) 
	{
		dnf_composer::log(dnf_composer::ERROR_, "Failed to write to destination file: " + destFileName);
		sourceFile.close();
		destFile.close(); // Close both files before returning
		return; // Exit the function if writing to the destination file fails
	}

	sourceFile.close();
	destFile.close();

	// Check the file sizes
	std::streampos sourceSize, destSize;
	std::ifstream sourceCheckFile(sourceFileName, std::ios::binary | std::ios::ate);
	std::ifstream destCheckFile(destFileName, std::ios::binary | std::ios::ate);

	do
	{
		sourceSize = sourceCheckFile.tellg();
		destSize = destCheckFile.tellg();
		//log(dnf_composer::DEBUG, "Checking if file size is correct... \n");
		//log(dnf_composer::DEBUG, "Source file size is: " + to_string(sourceSize) + " bytes. Destination file size is: " + to_string(destSize) + " bytes.\n");
	} while (sourceSize != destSize);

	dnf_composer::log(dnf_composer::INFO, "Original weights file loaded and copied successfully.\n");
}

void ExperimentHandler::createExperimentFolderDirectory() 
{
	log(dnf_composer::DEBUG, "ExperimentHandler::createExperimentFolderDirectory()\n");

	namespace fs = std::filesystem;

	const std::string experimentFolderPath = params.filePathPrefix + params.experimentId;
	fs::create_directory(experimentFolderPath);

	const std::string weightsFolderPath = params.filePathPrefix + params.experimentId + "/weights";
	fs::create_directory(weightsFolderPath);

	dnfcomposerHandler.setDataFilePath(weightsFolderPath);

	dnf_composer::log(dnf_composer::INFO, "Experiment folder directory created.\n");
}

void ExperimentHandler::deleteExperimentFolderDirectory() const
{
	log(dnf_composer::DEBUG, "ExperimentHandler::deleteExperimentFolderDirectory()\n");

	namespace fs = std::filesystem;

	const std::string experimentFolderPath = params.filePathPrefix + params.experimentId;
	fs::remove_all(experimentFolderPath);

	dnf_composer::log(dnf_composer::INFO, "Experiment folder directory deleted.\n");
}

void ExperimentHandler::saveWeights()
{
	log(dnf_composer::DEBUG, "ExperimentHandler::saveWeights()\n");

	dnfcomposerHandler.saveWeights();
	Sleep(300);
}

void ExperimentHandler::readWeights()
{
	log(dnf_composer::DEBUG, "ExperimentHandler::readWeights()\n");

	dnfcomposerHandler.readWeights();
	Sleep(300);
}