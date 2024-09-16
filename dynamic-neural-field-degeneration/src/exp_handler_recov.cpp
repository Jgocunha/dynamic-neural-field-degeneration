#include "exp_handler_recov.h"



namespace experiment
{
	namespace relearning
	{
		ExperimentHandlerRelearning::ExperimentHandlerRelearning()
		{
			dnfcomposerHandler.setRelearningParameters(parameters.relearningParameters.type, parameters.relearningParameters.numberOfEpochs, 
				parameters.relearningParameters.learningRate,parameters.relearningParameters.maxAmountOfDemonstrations, 
				parameters.relearningParameters.updateAllWeights);
			dnfcomposerHandler.setIncrementOfDegenerationPercentage(parameters.degenerationParameters.incrementOfDegenerationInPercentage);
			readHueToAngleMap();
			numberOfShapesPerTrial = static_cast<int>(hueToAngleMap.size());
		}

		void ExperimentHandlerRelearning::init()
		{
			parameters.id = "trials-" + std::to_string(parameters.numberOfTrials)
				+ " deg. type-" + parameters.degenerationParameters.name
				+ " target field-" + parameters.degenerationParameters.field
				+ " initial per.-" + std::to_string(parameters.degenerationParameters.initialPercentage)
				+ " inc per.-" + std::to_string(parameters.degenerationParameters.incrementOfDegenerationInPercentage)
				+ " epochs-" + std::to_string(parameters.relearningParameters.numberOfEpochs)
				+ " demos-" + std::to_string(parameters.relearningParameters.maxAmountOfDemonstrations);

			parameters.print();
			createExperimentFolderDirectory();
			getOriginalWeightsFile();
			dnfcomposerHandler.init();
			//if (parameters.isLinkToCoppeliaSimOn)
				//coppeliasimHandler.init();
			experimentThread = std::thread(&ExperimentHandlerRelearning::step, this);
		}

		void ExperimentHandlerRelearning::initialDegeneration()
		{
			mockPickAndPlace();

			int numberElements = 0;
			switch (parameters.degenerationParameters.type)
			{
			case degeneration::ElementDegeneracyType::NEURONS_DEACTIVATE:
				if (parameters.degenerationParameters.field == "perceptual")
					numberElements = 720 * parameters.degenerationParameters.initialPercentage / 100;
				if (parameters.degenerationParameters.field == "output")
					numberElements = 280 * parameters.degenerationParameters.initialPercentage / 100;
				break;
			case degeneration::ElementDegeneracyType::WEIGHTS_DEACTIVATE:
			case degeneration::ElementDegeneracyType::WEIGHTS_RANDOMIZE:
			case degeneration::ElementDegeneracyType::WEIGHTS_REDUCE:
				numberElements = 280 * 720 * parameters.degenerationParameters.initialPercentage / 100;
				break;
			case degeneration::ElementDegeneracyType::NONE:
				break;
			}

			dnfcomposerHandler.setInitialNumberOfElementsToDegenerate(numberElements);

			if (parameters.isDebugModeOn)
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::WARNING, "Number of not degenerated elements after pick and place: " + std::to_string(dnfcomposerHandler.getNumberOfDegeneratedElements()));

			//Sleep(100);

			degenerationProcedure();

			//Sleep(500);
			if (parameters.isDebugModeOn)
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::WARNING, "Number of not degenerated elements after degeneration: " + std::to_string(dnfcomposerHandler.getNumberOfDegeneratedElements()));


			currentPercentageOfDegeneration = parameters.degenerationParameters.initialPercentage;
			if (parameters.isDebugModeOn)
			{
				std::ostringstream logStream;
				logStream << "Degenerated to " << std::fixed << std::setprecision(2) << currentPercentageOfDegeneration << "%.\n";
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::INFO, logStream.str());
			}

			saveWeights();

			dnfcomposerHandler.setNumberOfElementsToDegenerate();
		}

		void ExperimentHandlerRelearning::step()
		{
			for (int trial = 1; trial <= parameters.numberOfTrials; trial++)
			{
				//Sleep(10);
				//dnfcomposerHandler.setWasStartSimulationRequested(true);
				//Sleep(100);

				if (parameters.degenerationParameters.initialPercentage != 0)
					initialDegeneration();

				do
				{
					bool successfulPickAndPlace = false;

					//if (parameters.isLinkToCoppeliaSimOn)
					//	successfulPickAndPlace = bonafidePickAndPlace();
					//else
						successfulPickAndPlace = mockPickAndPlace();

					if (successfulPickAndPlace || (statistics.numOfRelearningCycles >= parameters.relearningParameters.maxAmountOfDemonstrations))
					{
						if (doesBackupWeightsFileExist())
						{
							restoreWeightsFile();
							readWeights();
							saveWeights();
						}

						degenerationProcedure();
						saveWeights();

						if (statistics.numOfRelearningCycles >= parameters.relearningParameters.maxAmountOfDemonstrations)
						{
							data.isFieldDead = true;
							if (parameters.isDebugModeOn)
								dnf_composer::tools::logger::log(dnf_composer::tools::logger::INFO, "Re-learning did not work.\n");
						}
						statistics.learningCyclesPerTrialHistory.push_back(statistics.numOfRelearningCycles);
						statistics.numOfRelearningCycles = 0;
						currentPercentageOfDegeneration += parameters.degenerationParameters.incrementOfDegenerationInPercentage;
						if (parameters.isDebugModeOn)
						{
							std::ostringstream logStream;
							logStream << "Degenerated to " << std::fixed << std::setprecision(2) << currentPercentageOfDegeneration << "%.\n";
							dnf_composer::tools::logger::log(dnf_composer::tools::logger::INFO, logStream.str());
						}
					}
					else
					{
						if (!doesBackupWeightsFileExist())
							backupWeightsFile();
						relearningProcedure();
					}
					cleanupPickAndPlace();

				} while ((currentPercentageOfDegeneration < parameters.degenerationParameters.targetPercentage) && !(data.isFieldDead));

				cleanupTrial();
			}
			dnfcomposerHandler.stop();
			deleteExperimentFolderDirectory();
		}

		void ExperimentHandlerRelearning::close()
		{
			dnfcomposerHandler.close();
			//if (parameters.isLinkToCoppeliaSimOn)
				//coppeliasimHandler.close();
		}

		//bool ExperimentHandlerRelearning::bonafidePickAndPlace()
		//{
		//	statistics.shapesPlacedIncorrectly = 0; // binary representation
		//	bool successfulPickAndPlace = true;

		//	for (int i = 0; i < parameters.numberOfShapesPerTrial; i++)
		//	{
		//		createShape();
		//		readShapeHue();
		//		readTargetAngle();
		//		if (!verifyDecision())
		//			successfulPickAndPlace = false;
		//		graspShape();
		//		placeShape();
		//		coppeliasimHandler.resetSignals();
		//	}

		//	if (parameters.isDebugModeOn)
		//	{
		//		dnf_composer::log(dnf_composer::tools::logger::LogLevel::INFO, "Binary representation of placed boxes: " + std::bitset<7>(statistics.shapesPlacedIncorrectly).to_string() + '\n');
		//		std::ostringstream logStream;
		//		logStream << "Pick and place procedure finished, with" << (successfulPickAndPlace ? " success." : "out success.") << std::endl;
		//		dnf_composer::log(dnf_composer::tools::logger::LogLevel::INFO, logStream.str());
		//	}

		//	return successfulPickAndPlace;
		//}

		//void ExperimentHandlerRelearning::createShape()
		//{
		//	// set the create shape signal to true
		//	signals.createShape = true;
		//	coppeliasimHandler.setSignals(signals);
		//	signals.createShape = false;

		//	// wait for the shape created signal to be true
		//	while (!coppeliasimHandler.getSignals().isShapeCreated);
		//}

		//void ExperimentHandlerRelearning::graspShape()
		//{
		//	// go pick up the cuboid
		//	signals.graspShape = true;
		//	coppeliasimHandler.setSignals(signals);
		//	signals.graspShape = false;

		//	// wait for the cuboid to be grasped
		//	while (!coppeliasimHandler.getSignals().isShapeGrasped);
		//}

		//void ExperimentHandlerRelearning::placeShape()
		//{
		//	// and set place shape to true
		//	signals.placeShape = true;
		//	coppeliasimHandler.setSignals(signals);
		//	signals.placeShape = false;
		//	signals.targetAngle = UNDEFINED;

		//	// when receive shape placed restart cycle
		//	while (!coppeliasimHandler.getSignals().isShapePlaced);
		//	coppeliasimHandler.setSignals(signals);
		//}

		bool ExperimentHandlerRelearning::verifyDecision()
		{
			bool isCorrectDecision = std::abs(data.expectedTargetAngle - data.outputFieldCentroid) <= parameters.decisionTolerance;
			if (isCorrectDecision)
			{
				statistics.shapesPlacedIncorrectly = (statistics.shapesPlacedIncorrectly << 1) | 1;
				return true;
			}
			statistics.shapesPlacedIncorrectly = statistics.shapesPlacedIncorrectly << 1;
			return false;
		}

		//void ExperimentHandlerRelearning::readShapeHue()
		//{
		//	// wait for the hue of the cuboid
		//	do
		//	{
		//		Sleep(50); // necessary?
		//		signals.shapeHue = coppeliasimHandler.getSignals().shapeHue;
		//		//if (parameters.isDebugModeOn)
		//			//std::cout << "Shape hue: " << signals.shapeHue << std::endl;
		//	} while (signals.shapeHue == UNDEFINED);

		//	// set the hue of the cuboid for dnfcomposer
		//	dnfcomposerHandler.setExternalInput(signals.shapeHue);
		//	data.shapeHue = signals.shapeHue;
		//	signals.shapeHue = UNDEFINED;

		//	// wait for the shape hue to be read
		//	while (!dnfcomposerHandler.getHaveFieldsSettled());
		//	dnfcomposerHandler.setHaveFieldsSettled(false);
		//}

		//void ExperimentHandlerRelearning::readTargetAngle()
		//{
		//	dnfcomposerHandler.updateFieldCentroids();
		//	Sleep(10);
		//	signals.targetAngle = dnfcomposerHandler.getOutputFieldCentroid();
		//	//if (parameters.isDebugModeOn)
		//		//std::cout << "Target angle: " << signals.targetAngle << std::endl;

		//	data.outputFieldCentroid = signals.targetAngle;
		//	data.lastOutputFieldCentroid = signals.targetAngle;

		//	getExpectedTargetAngle();
		//	dnfcomposerHandler.updateFieldCentroids();

		//	// set the target angle for CoppeliaSim
		//	coppeliasimHandler.setSignals(signals);
		//}

		void ExperimentHandlerRelearning::getExpectedTargetAngle()
		{
			// Check if cuboidHue exists in the map
			auto closestHueIter = hueToAngleMap.end();
			double minDifference = parameters.decisionTolerance;

			for (auto it = hueToAngleMap.begin(); it != hueToAngleMap.end(); ++it)
			{
				double difference = std::abs(data.shapeHue - it->first);
				if (difference <= parameters.decisionTolerance && difference < minDifference)
				{
					minDifference = difference;
					closestHueIter = it;
				}
			}

			if (closestHueIter != hueToAngleMap.end())
				data.expectedTargetAngle = closestHueIter->second;
		}

		bool ExperimentHandlerRelearning::mockPickAndPlace()
		{
			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Performing a mock pick and place.\n");

			statistics.shapesPlacedIncorrectly = 0; // binary representation
			bool successfulPickAndPlace = true;

			for (int i = 0; i < numberOfShapesPerTrial; i++)
			{
				mockReadShapeHue();
				mockReadTargetAngle();
				if (!verifyDecision())
					successfulPickAndPlace = false;
			}

			if (parameters.isDebugModeOn)
			{
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Binary representation of placed boxes: " + std::bitset<7>(statistics.shapesPlacedIncorrectly).to_string() + '\n');
				std::ostringstream logStream;
				logStream << "Pick and place procedure finished, with" << (successfulPickAndPlace ? " success." : "out success.") << std::endl;
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, logStream.str());
			}

			return successfulPickAndPlace;
		}

		void ExperimentHandlerRelearning::mockReadShapeHue()
		{
			if (hueToAngleIterator == hueToAngleMap.end())
				hueToAngleIterator = hueToAngleMap.begin();

			data.shapeHue = hueToAngleIterator->first;
			data.expectedTargetAngle = hueToAngleIterator->second;
			//if (parameters.isDebugModeOn)
				//std::cout << "External stimulus: " << data.shapeHue << std::endl;
			++hueToAngleIterator;

			dnfcomposerHandler.setExternalInput(data.shapeHue);

			while (!dnfcomposerHandler.getHaveFieldsSettled());
			dnfcomposerHandler.setHaveFieldsSettled(false);
		}

		void ExperimentHandlerRelearning::mockReadTargetAngle()
		{
			//dnfcomposerHandler.updateFieldCentroids();
			//Sleep(5);
			signals.targetAngle = dnfcomposerHandler.getOutputFieldCentroid();
			//if (parameters.isDebugModeOn)
				//std::cout << "Target angle: " << signals.targetAngle << std::endl;

			data.outputFieldCentroid = signals.targetAngle;
			data.lastOutputFieldCentroid = signals.targetAngle;

			getExpectedTargetAngle();
			//dnfcomposerHandler.updateFieldCentroids();
		}

		void ExperimentHandlerRelearning::degenerationProcedure()
		{
			if (parameters.isDebugModeOn)
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Degeneration procedure started.\n");

			// Disable the user interface whilst degenerating to consume less time.
			if (parameters.isVisualizationOn)
				dnfcomposerHandler.setIsUserInterfaceActiveAs(false);

			dnfcomposerHandler.setDegeneracy(parameters.degenerationParameters.type, parameters.degenerationParameters.field);
			while (!dnfcomposerHandler.getHaveFieldsSettled());
			dnfcomposerHandler.setHaveFieldsSettled(false);

			// Re-enable the UI.
			if (parameters.isVisualizationOn)
				dnfcomposerHandler.setIsUserInterfaceActiveAs(true);
		}

		void ExperimentHandlerRelearning::relearningProcedure()
		{
			// make sure to test two alternatives
			// 1. use the 7 inputs
			// 2. use only the inputs from the incorrect correspondence
			// Here we can also test running for 1 iteration vs. 100 iterations per learning cycle
			// And the learning rate
			//log(dnf_composer::DEBUG, "ExperimentHandler::relearningProcedure()\n");

			if (parameters.isDebugModeOn)
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Relearning procedure started.\n");

			dnfcomposerHandler.setRelearning(statistics.shapesPlacedIncorrectly);

			while (!dnfcomposerHandler.getHasRelearningFinished());
			dnfcomposerHandler.setHasRelearningFinished(false);

			if (parameters.isDebugModeOn)
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Relearning procedure finished.\n");

			statistics.numOfRelearningCycles++;
		}

		void ExperimentHandlerRelearning::cleanupPickAndPlace()
		{
			statistics.shapesPlacedIncorrectly = 0;
			//if (parameters.isLinkToCoppeliaSimOn)
				//coppeliasimHandler.resetSignals();
			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Pick and place procedure finished.\n");
		}

		void ExperimentHandlerRelearning::cleanupTrial()
		{
			if (parameters.isDataSavingOn)
				saveLearningCyclesPerTrial();
			statistics.learningCyclesPerTrialHistory.clear();
			currentPercentageOfDegeneration = 0;
			data.isFieldDead = false;
			getOriginalWeightsFile();
			dnfcomposerHandler.setWasCloseSimulationRequested(true);
			Sleep(300);
			//Sleep(300);
			//dnfcomposerHandler.setWasStartSimulationRequested(true);
			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Trial finished.\n");
		}

		void ExperimentHandlerRelearning::saveLearningCyclesPerTrial() const
		{
			const std::string filename = parameters.getSavePath();

			std::ofstream file(filename, std::ios::app);

			if (file.is_open()) {
				for (const int cycles : statistics.learningCyclesPerTrialHistory)
					file << cycles << " "; 

				file << "\n";

				file.flush();

				if (file.good()) {
					file.close();
					dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Number of relearning cycles needed saved to file.\n");
				}
				else {
					dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::ERROR, "Error while writing to file: " + filename + '\n');
				}
			}
			else {
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::ERROR, "Unable to open file: " + filename + '\n');
			}
		}

		void ExperimentHandlerRelearning::backupWeightsFile() const
		{
			const std::string newFilename = "per - out_weights - copy.txt";
			std::string filename = std::string(OUTPUT_DIRECTORY) + "/" + parameters.id + "/weights/" + "per - out_weights.txt";
			std::string filenameCopy = std::string(OUTPUT_DIRECTORY) + "/" + parameters.id + "/weights/" + newFilename;

			std::ifstream source(filename, std::ios::binary);
			std::ofstream dest(filenameCopy, std::ios::binary);

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Attempting to back up weights file...\n");

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
				////log(dnf_composer::DEBUG, "Checking if file size is correct... \n");
				////log(dnf_composer::DEBUG, "Source file size is: " + to_string(sourceSize) + " bytes. Destination file size is: " + to_string(destSize) + " bytes.\n");
			} while (sourceSize != destSize);

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Weights file successfully backed-up.\n");

		}

		void ExperimentHandlerRelearning::restoreWeightsFile() const
		{
			const std::string oldFilename = std::string(OUTPUT_DIRECTORY) + "/" + parameters.id + "/weights/" + "per - out_weights - copy.txt";
			const std::string newFilename = std::string(OUTPUT_DIRECTORY) + "/" + parameters.id + "/weights/" + "per - out_weights.txt";

			////log(dnf_composer::DEBUG, "Attempting to restore weights file...\n");

			// remove weights file
			const int removeResult = std::remove(newFilename.c_str());
			std::ifstream oldFile(newFilename.c_str());
			do
			{
				//(dnf_composer::DEBUG, "Checking if weights file still exists...\n");
				oldFile.open(newFilename.c_str());
			} while (oldFile.is_open());
			oldFile.close();
			//dnf_composer::log(dnf_composer::tools::logger::LogLevel::INFO, "Previous weights file successfully deleted.\n");

			// rename copy of weights file
			std::ifstream currentFile(oldFilename.c_str(), std::ios::binary | std::ios::ate);
			std::streampos sourceSize, destSize;
			sourceSize = currentFile.tellg();

			currentFile.close();
			const int renameResult = std::rename(oldFilename.c_str(), newFilename.c_str());

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "File successfully renamed.\n");

			std::ifstream newFile(newFilename.c_str(), std::ios::binary | std::ios::ate);
			do
			{
				destSize = newFile.tellg();
				//log(dnf_composer::DEBUG, "Checking if file size is correct... \n");
				//log(dnf_composer::DEBUG, "Source file size is: " + to_string(sourceSize) + " bytes. Destination file size is: " + to_string(destSize) + " bytes.\n");
			} while (sourceSize != destSize);

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Weights file successfully restored.\n");
		}

		bool ExperimentHandlerRelearning::doesBackupWeightsFileExist() const
		{
			const std::string filename = std::string(OUTPUT_DIRECTORY) + "/" + parameters.id + "/weights/" + "per - out_weights - copy.txt";
			const std::ifstream file(filename);

			if (file.good())
			{
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Weights file exists.\n");
				return true;
			}

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Weights file does not exist.\n");
			return false;
		}

		void ExperimentHandlerRelearning::getOriginalWeightsFile() const
		{
			const std::string sourceFileName = std::string(OUTPUT_DIRECTORY) + "/weights-backup/per - out_weights.txt";
			std::ifstream sourceFile(sourceFileName);

			// //log(dnf_composer::DEBUG, "Attempting to get original weights file...\n");

			if (!sourceFile.is_open())
			{
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::ERROR, "Failed to open source file: " + sourceFileName);
				return; // Exit the function if the source file cannot be opened
			}

			const std::string destFileName = std::string(OUTPUT_DIRECTORY) + "/" + parameters.id + "/weights/" + "per - out_weights.txt";
			std::ofstream destFile(destFileName);

			if (!destFile.is_open())
			{
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::ERROR, "Failed to open destination file: " + destFileName);
				sourceFile.close(); // Close the source file before returning
				return; // Exit the function if the destination file cannot be opened
			}

			destFile << sourceFile.rdbuf();

			if (destFile.fail())
			{
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::ERROR, "Failed to write to destination file: " + destFileName);
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

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Original weights file loaded and copied successfully.\n");
		}

		void ExperimentHandlerRelearning::createExperimentFolderDirectory()
		{
			namespace fs = std::filesystem;

			const std::string experimentFolderPath = std::string(OUTPUT_DIRECTORY) + "/" + parameters.id;
			fs::create_directory(experimentFolderPath);

			const std::string weightsFolderPath = std::string(OUTPUT_DIRECTORY) + "/" + parameters.id + "/weights";
			fs::create_directory(weightsFolderPath);

			dnfcomposerHandler.setDataFilePath(weightsFolderPath);

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Experiment folder directory created.\n");
		}

		void ExperimentHandlerRelearning::deleteExperimentFolderDirectory() const
		{
			namespace fs = std::filesystem;

			const std::string experimentFolderPath = std::string(OUTPUT_DIRECTORY) + "/" + parameters.id;
			fs::remove_all(experimentFolderPath);

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Experiment folder directory deleted.\n");
		}

		void ExperimentHandlerRelearning::saveWeights()
		{
			dnfcomposerHandler.saveWeights();
			Sleep(300);
		}

		void ExperimentHandlerRelearning::readWeights()
		{
			dnfcomposerHandler.readWeights();
			Sleep(300);
		}

		void ExperimentHandlerRelearning::readHueToAngleMap()
		{
			std::ifstream file(std::string(PROJECT_DIR) + "/hue_to_angle.json");

			if (!file.is_open()) {
				std::cerr << "Error: Could not open the JSON colors file." << std::endl;
			}

			nlohmann::json j;
			file >> j;

			for (auto& [key, value] : j.items()) {

				if (key == "metadata") {
					continue;
				}

				double hue = std::stod(key);
				const int angle = value;
				hueToAngleMap[hue] = angle;
			}

			file.close();
		}
	}
}
