#include "exp_handler_ind.h"

namespace experiment
{
	namespace degeneration
	{
		ExperimentHandlerInducing::ExperimentHandlerInducing()
			: params(), dnfcomposerHandler(params.isVisualizationOn)
		{
			data.outputFieldCentroidHistory.reserve(60000);
			std::advance(hueToAngleIterator, params.startingExternalStimulus);
			readHueToAngleMap();
		}


		void ExperimentHandlerInducing::setExpectedFieldBehaviour()
		{
			if (hueToAngleIterator == hueToAngleMap.end())
				hueToAngleIterator = hueToAngleMap.begin();

			data.targetInputFieldCentroid = hueToAngleIterator->first;
			data.targetOutputFieldCentroid = hueToAngleIterator->second;

			++hueToAngleIterator;
		}

		void ExperimentHandlerInducing::setExperimentAsEnded()
		{
			dnfcomposerHandler.stop();
		}

		void ExperimentHandlerInducing::init()
		{
			dnfcomposerHandler.init();
			experimentThread = std::thread(&ExperimentHandlerInducing::step, this);
		}

		void ExperimentHandlerInducing::setExperimentSetupData()
		{
			dnfcomposerHandler.setNumberOfElementsToDegenerate(params.degenerationParameters.numberOfElementsToDegeneratePerIteration);
		}

		void ExperimentHandlerInducing::step()
		{
			params.print();
			setExperimentSetupData();

			for (int i = 0; i < params.numberOfTrials; i++)
			{
				params.currentTrial = i + 1;
				if (params.isDebugModeOn)
				{
					std::string message = "Starting trial " + std::to_string(params.currentTrial) + " out of " + std::to_string(params.numberOfTrials) + ". ";
					message += "External stimulus: " + std::to_string(data.targetInputFieldCentroid) + ". ";
					message += "Expected input field centroid: " + std::to_string(data.targetInputFieldCentroid) + ". ";
					message += "Expected output field centroid: " + std::to_string(data.targetOutputFieldCentroid) + ".";
					dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, message);
				}

				for (int k = 0; k < static_cast<int>(hueToAngleMap.size()); k++)
				{
					setExpectedFieldBehaviour();
					setupProcedure();
					degenerationProcedure();
					cleanUpTrial();
					Sleep(20);
				}
				Sleep(50);
			}
			setExperimentAsEnded();
		}

		void ExperimentHandlerInducing::close()
		{
			experimentThread.join();
			dnfcomposerHandler.close();
		}

		void ExperimentHandlerInducing::setupProcedure()
		{
			// add and remove stimulus and wait for the fields to settle
			dnfcomposerHandler.setExternalInput(data.targetInputFieldCentroid);

			if (params.isDebugModeOn)
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::INFO, "Added gaussian stimulus to perceptual field.");

			while (!dnfcomposerHandler.getHaveFieldsSettled());
			dnfcomposerHandler.setHaveFieldsSettled(false);
		}

		void ExperimentHandlerInducing::degenerationProcedure()
		{
			bool isOutputFieldDegenerated = hasOutputFieldDegenerated();

			while (!isOutputFieldDegenerated)
			{
				// save centroid of the output field
				Sleep(2);
				data.outputFieldCentroidHistory.push_back(dnfcomposerHandler.getOutputFieldCentroid());

				// apply degeneration and wait for the fields to settle
				dnfcomposerHandler.setDegeneracy(params.degenerationParameters.type, params.degenerationParameters.field);
				if (params.isDebugModeOn)
				{
					std::string message = "Trial: " + std::to_string(params.currentTrial) + ". ";
					message += "Number of degenerated " + params.degenerationParameters.name + ": "
						+ std::to_string(data.outputFieldCentroidHistory.size() * params.degenerationParameters.numberOfElementsToDegeneratePerIteration) + "/" + std::to_string(params.degenerationParameters.totalNumberOfElementsToDegenerate)
						+ " (" + std::to_string(static_cast<int>(static_cast<double>(data.outputFieldCentroidHistory.size() * params.degenerationParameters.numberOfElementsToDegeneratePerIteration) / params.degenerationParameters.totalNumberOfElementsToDegenerate * 100)) + "%%). ";

					std::ostringstream stream;
					stream << std::fixed << std::setprecision(2);

					// Function to compute circular deviation
					auto circularDeviation = [](double a, double b, double range) -> double {
						double diff = std::abs(a - b);
						return std::min(diff, range - diff);
						};

					// Perceptual field (range 0-360)
					const double perceptualCentroid = dnfcomposerHandler.getInputFieldCentroid();
					const double targetPerceptualCentroid = data.targetInputFieldCentroid;
					const double perceptualDeviation = circularDeviation(perceptualCentroid, targetPerceptualCentroid, 360.0);

					// Output field (range 0-28)
					const double outputCentroid = dnfcomposerHandler.getOutputFieldCentroid();
					const double targetOutputCentroid = data.targetOutputFieldCentroid;
					const double outputDeviation = circularDeviation(outputCentroid, targetOutputCentroid, 28.0);

					// Stream output
					stream << "Perceptual field centroid is " << perceptualCentroid << " and should be " << targetPerceptualCentroid
						<< " (deviation of " << perceptualDeviation << "). ";
					stream << "Output field centroid is " << outputCentroid << " and should be " << targetOutputCentroid
						<< " (deviation of " << outputDeviation << ").";

					message += stream.str();
					log(dnf_composer::tools::logger::INFO, message);
				}

				while (!dnfcomposerHandler.getHaveFieldsSettled());

				dnfcomposerHandler.setHaveFieldsSettled(false);

				isOutputFieldDegenerated = hasOutputFieldDegenerated();
			}
		}

		void ExperimentHandlerInducing::cleanUpTrial()
		{
			if (params.isDataSavingOn)
				saveOutputFieldCentroidToFile();
			Sleep(20);
			data.outputFieldCentroidHistory.clear();
			dnfcomposerHandler.closeSimulation();
		}

		bool ExperimentHandlerInducing::hasOutputFieldDegenerated() const
		{
			const double outputFieldCentroid = dnfcomposerHandler.getOutputFieldCentroid();

			if (outputFieldCentroid < 0)
				return true;

			return false;
		}

		void ExperimentHandlerInducing::saveOutputFieldCentroidToFile() const
		{
			std::ostringstream ss;
			ss << std::fixed << std::setprecision(1) << data.targetOutputFieldCentroid;
			const std::string decimalString = ss.str();

			const std::string filename = std::string(OUTPUT_DIRECTORY) + "/results/" + decimalString + " " + params.degenerationParameters.name + " - centroids.txt";
			std::ofstream file(filename, std::ios::app);

			if (!file.is_open())
			{
				if (params.isDebugModeOn)
				{
					const std::string message = "Failed to open the file for writing " + filename + '.';
					dnf_composer::tools::logger::log(dnf_composer::tools::logger::FATAL, message);
				}
			}

			for (const auto& centroid : data.outputFieldCentroidHistory)
				file << centroid << " ";
			file << std::endl;

			file.close();

			if (params.isDebugModeOn)
			{
				const std::string message = "New centroids appended to " + filename + '.';
				dnf_composer::tools::logger::log(dnf_composer::tools::logger::INFO, message);
			}
		}

		void ExperimentHandlerInducing::readHueToAngleMap()
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