#include "exp_handler_recov.h"



namespace experiment
{
	namespace relearning
	{
		ExperimentHandlerRelearning::ExperimentHandlerRelearning()
		{
			dnfcomposerHandler.setRelearningParameters(parameters.relearningParameters.type, parameters.relearningParameters.numberOfEpochs,
				parameters.relearningParameters.learningRate, parameters.relearningParameters.maxAmountOfDemonstrations,
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
			dnf_composer::tools::logger::Logger::setMinLogLevel(dnf_composer::tools::logger::ERROR);
			createExperimentFolderDirectory();
			getOriginalWeightsFile();
			dnfcomposerHandler.setIsUserInterfaceActiveAs(parameters.isVisualizationOn);
			dnfcomposerHandler.init();
			if (parameters.isLinkToCoppeliaSimOn)
				coppeliasimHandler.init();
			experimentThread = std::thread(&ExperimentHandlerRelearning::step, this);
		}

		void ExperimentHandlerRelearning::initialDegeneration()
		{
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
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::WARNING, "Number of not degenerated elements before degeneration: " + std::to_string(dnfcomposerHandler.getNumberOfDegeneratedElements()));

			degenerationProcedure();

			if (parameters.isDebugModeOn)
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::WARNING, "Number of not degenerated elements after degeneration: " + std::to_string(dnfcomposerHandler.getNumberOfDegeneratedElements()));


			currentPercentageOfDegeneration = parameters.degenerationParameters.initialPercentage;
			if (parameters.isDebugModeOn)
			{
				std::ostringstream logStream;
				logStream << "Degenerated to " << std::fixed << std::setprecision(2) << currentPercentageOfDegeneration << "%.";
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::INFO, logStream.str());
			}

			saveWeights();

			dnfcomposerHandler.setNumberOfElementsToDegenerate();
		}

		void ExperimentHandlerRelearning::step()
		{
			for (int trial = 1; trial <= parameters.numberOfTrials; trial++)
			{
				bonafidePickAndPlace();
				if (parameters.degenerationParameters.initialPercentage != 0)
					initialDegeneration();

				dnfcomposerHandler.setDegeneracy(parameters.degenerationParameters.type, parameters.degenerationParameters.field);
				dnfcomposerHandler.setNumberOfElementsToDegenerate();

				do
				{
					const bool successfulPickAndPlace = bonafidePickAndPlace();

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
						}
						statistics.learningCyclesPerTrialHistory.push_back(statistics.numOfRelearningCycles);
						statistics.numOfRelearningCycles = 0;
						currentPercentageOfDegeneration += parameters.degenerationParameters.incrementOfDegenerationInPercentage;

						std::ostringstream logStream;
						logStream << "(relearning-experiment) Degenerated to " << std::fixed << std::setprecision(2) << currentPercentageOfDegeneration << "%.";
						dnf_composer::tools::logger::log(dnf_composer::tools::logger::INFO, logStream.str());
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
			if (experimentThread.joinable())
				experimentThread.join();
			dnfcomposerHandler.close();
		}

		bool ExperimentHandlerRelearning::bonafidePickAndPlace()
		{
			statistics.shapesPlacedIncorrectly = 0; // binary representation
			bool successfulPickAndPlace = true;

			for (int i = 0; i < numberOfShapesPerTrial; i++)
			{
				createShape();
				mockReadShapeHue();
				signals.shapeHue = data.shapeHue;
				coppeliasimHandler.setSignals(signals);
				Sleep(2000);
				mockReadTargetAngle();
				signals.targetAngle = data.outputFieldCentroid;
				coppeliasimHandler.setSignals(signals);
				if (!verifyDecision())
					successfulPickAndPlace = false;
				graspShape();
				placeShape();
				coppeliasimHandler.resetSignals();
				Sleep(100);
			}

			if (parameters.isDebugModeOn)
			{
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "(relearning-experiment) Binary representation of placed boxes: " + std::bitset<7>(statistics.shapesPlacedIncorrectly).to_string() + '.');
				std::ostringstream logStream;
				logStream << "(relearning-experiment) Pick and place procedure finished, with" << (successfulPickAndPlace ? " success." : "out success.");
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, logStream.str());
			}

			return successfulPickAndPlace;
		}

		void ExperimentHandlerRelearning::createShape()
		{
			// set the create shape signal to true
			signals.createShape = true;
			coppeliasimHandler.setSignals(signals);
			signals.createShape = false;

			// wait for the shape created signal to be true
			while (!coppeliasimHandler.getSignals().isShapeCreated);
		}

		void ExperimentHandlerRelearning::graspShape()
		{
			// go pick up the cuboid
			signals.graspShape = true;
			coppeliasimHandler.setSignals(signals);
			signals.graspShape = false;

			// wait for the cuboid to be grasped
			while (!coppeliasimHandler.getSignals().isShapeGrasped);
		}

		void ExperimentHandlerRelearning::placeShape()
		{
			// and set place shape to true
			signals.placeShape = true;
			coppeliasimHandler.setSignals(signals);
			signals.placeShape = false;
			signals.targetAngle = -1;

			// when receive shape placed restart cycle
			while (!coppeliasimHandler.getSignals().isShapePlaced);
			coppeliasimHandler.setSignals(signals);
		}

		bool ExperimentHandlerRelearning::verifyDecision()
		{
			const bool isCorrectDecision = std::abs(data.expectedTargetAngle - data.outputFieldCentroid) <= parameters.decisionTolerance;
			if (isCorrectDecision)
			{
				statistics.shapesPlacedIncorrectly = (statistics.shapesPlacedIncorrectly << 1) | 1;
				return true;
			}
			statistics.shapesPlacedIncorrectly = statistics.shapesPlacedIncorrectly << 1;
			return false;
		}

		void ExperimentHandlerRelearning::readShapeHue()
		{
			// wait for the hue of the cuboid
			do
			{
				signals.shapeHue = coppeliasimHandler.getSignals().shapeHue;
			} while (signals.shapeHue < 0);

			// set the hue of the cuboid for dnfcomposer
			dnfcomposerHandler.setExternalInput(signals.shapeHue);
			data.shapeHue = signals.shapeHue;
			signals.shapeHue = -1;

			// wait for the shape hue to be read
			Sleep(20);
			dnfcomposerHandler.setHaveFieldsSettled(false);
		}

		void ExperimentHandlerRelearning::readTargetAngle()
		{
			signals.targetAngle = dnfcomposerHandler.getOutputFieldCentroid();
			signals.targetAngle = -1;
			do
			{
				signals.targetAngle = dnfcomposerHandler.getOutputFieldCentroid();
			} while (signals.targetAngle < 0);

			data.outputFieldCentroid = signals.targetAngle;
			data.lastOutputFieldCentroid = signals.targetAngle;

			getExpectedTargetAngle();
			coppeliasimHandler.setSignals(signals);
		}

		void ExperimentHandlerRelearning::getExpectedTargetAngle()
		{
			// Check if cuboidHue exists in the map
			auto closestHueIter = hueToAngleMap.end();
			double minDifference = parameters.decisionTolerance;

			for (auto it = hueToAngleMap.begin(); it != hueToAngleMap.end(); ++it)
			{
				const double difference = std::abs(data.shapeHue - it->first);
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
			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "(relearning-experiment) Performing a mock pick and place.");

			statistics.shapesPlacedIncorrectly = 0; // binary representation
			bool successfulPickAndPlace = true;

			for (int i = 0; i < numberOfShapesPerTrial; i++)
			{
				mockReadShapeHue();
				mockReadTargetAngle();
				if (!verifyDecision())
					successfulPickAndPlace = false;
			}

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "(relearning-experiment) Binary representation of placed boxes: " + std::bitset<7>(statistics.shapesPlacedIncorrectly).to_string() + '.');
			std::ostringstream logStream;
			logStream << "(relearning-experiment) Pick and place procedure finished, with" << (successfulPickAndPlace ? " success." : "out success.") << std::endl;
			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, logStream.str());

			return successfulPickAndPlace;
		}

		void ExperimentHandlerRelearning::mockReadShapeHue()
		{
			if (hueToAngleIterator == hueToAngleMap.end())
				hueToAngleIterator = hueToAngleMap.begin();
			data.shapeHue = hueToAngleIterator->first;
			data.expectedTargetAngle = hueToAngleIterator->second;
			++hueToAngleIterator;
			dnfcomposerHandler.setExternalInput(data.shapeHue);
			while (!dnfcomposerHandler.getHaveFieldsSettled());
			dnfcomposerHandler.setHaveFieldsSettled(false);
		}

		void ExperimentHandlerRelearning::mockReadTargetAngle()
		{
			signals.targetAngle = dnfcomposerHandler.getOutputFieldCentroid();
			data.outputFieldCentroid = signals.targetAngle;
			data.lastOutputFieldCentroid = signals.targetAngle;

			getExpectedTargetAngle();
		}

		void ExperimentHandlerRelearning::degenerationProcedure()
		{
			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "(relearning-experiment) Degeneration procedure started.");

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
			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "(relearning-experiment) Relearning procedure started.");

			dnfcomposerHandler.setRelearning(statistics.shapesPlacedIncorrectly);

			while (!dnfcomposerHandler.getHasRelearningFinished());
			dnfcomposerHandler.setHasRelearningFinished(false);

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "(relearning-experiment) Relearning procedure finished.");

			statistics.numOfRelearningCycles++;
		}

		void ExperimentHandlerRelearning::cleanupPickAndPlace()
		{
			hueToAngleIterator = hueToAngleMap.begin();
			statistics.shapesPlacedIncorrectly = 0;
			if (parameters.isLinkToCoppeliaSimOn)
				coppeliasimHandler.resetSignals();
			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "(relearning-experiment) Pick and place procedure finished.");
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
			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "(relearning-experiment) Trial finished.");
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
					dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Number of relearning cycles needed saved to file.");
				}
				else {
					dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::ERROR, "Error while writing to file: " + filename + '.');
				}
			}
			else {
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::ERROR, "Unable to open file: " + filename + '.');
			}
		}

		void ExperimentHandlerRelearning::backupWeightsFile() const
		{
			const std::string newFilename = "per - out_weights - copy.txt";
			std::string filename = std::string(OUTPUT_DIRECTORY) + "/" + parameters.id + "/weights/" + "per - out_weights.txt";
			std::string filenameCopy = std::string(OUTPUT_DIRECTORY) + "/" + parameters.id + "/weights/" + newFilename;

			std::ifstream source(filename, std::ios::binary);
			std::ofstream dest(filenameCopy, std::ios::binary);

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Attempting to back up weights file...");

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
			} while (sourceSize != destSize);

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Weights file successfully backed-up.");

		}

		void ExperimentHandlerRelearning::restoreWeightsFile() const
		{
			const std::string oldFilename = std::string(OUTPUT_DIRECTORY) + "/" + parameters.id + "/weights/" + "per - out_weights - copy.txt";
			const std::string newFilename = std::string(OUTPUT_DIRECTORY) + "/" + parameters.id + "/weights/" + "per - out_weights.txt";

			// remove weights file
			const int removeResult = std::remove(newFilename.c_str());
			std::ifstream oldFile(newFilename.c_str());
			do
			{
				oldFile.open(newFilename.c_str());
			} while (oldFile.is_open());
			oldFile.close();

			// rename copy of weights file
			std::ifstream currentFile(oldFilename.c_str(), std::ios::binary | std::ios::ate);
			std::streampos sourceSize, destSize;
			sourceSize = currentFile.tellg();

			currentFile.close();
			const int renameResult = std::rename(oldFilename.c_str(), newFilename.c_str());

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "File successfully renamed.");

			std::ifstream newFile(newFilename.c_str(), std::ios::binary | std::ios::ate);
			do
			{
				destSize = newFile.tellg();
			} while (sourceSize != destSize);

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Weights file successfully restored.");
		}

		bool ExperimentHandlerRelearning::doesBackupWeightsFileExist() const
		{
			const std::string filename = std::string(OUTPUT_DIRECTORY) + "/" + parameters.id + "/weights/" + "per - out_weights - copy.txt";
			const std::ifstream file(filename);

			if (file.good())
			{
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Weights file exists.");
				return true;
			}

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Weights file does not exist.");
			return false;
		}

		void ExperimentHandlerRelearning::getOriginalWeightsFile() const
		{
			const std::string sourceFileName = std::string(OUTPUT_DIRECTORY) + "/weights-backup/per - out_weights.txt";
			std::ifstream sourceFile(sourceFileName);

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
			} while (sourceSize != destSize);

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Original weights file loaded and copied successfully.");
		}

		void ExperimentHandlerRelearning::createExperimentFolderDirectory()
		{
			namespace fs = std::filesystem;

			const std::string experimentFolderPath = std::string(OUTPUT_DIRECTORY) + "/" + parameters.id;
			fs::create_directory(experimentFolderPath);

			const std::string weightsFolderPath = std::string(OUTPUT_DIRECTORY) + "/" + parameters.id + "/weights";
			fs::create_directory(weightsFolderPath);

			dnfcomposerHandler.setDataFilePath(weightsFolderPath);

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Experiment folder directory created.");
		}

		void ExperimentHandlerRelearning::deleteExperimentFolderDirectory() const
		{
			namespace fs = std::filesystem;

			const std::string experimentFolderPath = std::string(OUTPUT_DIRECTORY) + "/" + parameters.id;
			fs::remove_all(experimentFolderPath);

			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, "Experiment folder directory deleted.");
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