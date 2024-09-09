
#include "experiment_handler.h"

ExperimentHandler::ExperimentHandler()
{
	data.outputFieldCentroidHistory.reserve(60000);
	std::advance(hueToAngleIterator, params.startingExternalStimulus);
	data.targetInputFieldCentroid = hueToAngleIterator->first;
	data.targetOutputFieldCentroid = hueToAngleIterator->second;
}

 void ExperimentHandler::printExperimentSetupToConsole()
{
	 params.print();
}


 void ExperimentHandler::setExpectedFieldBehaviour()
 {
	 if (hueToAngleIterator == hueToAngleMap.end())
		 hueToAngleIterator = hueToAngleMap.begin();

	 data.targetInputFieldCentroid = hueToAngleIterator->first;
	 data.targetOutputFieldCentroid = hueToAngleIterator->second;

	 //if (params.isDebugModeOn)
		 //std::cout << "External stimulus: " << data.shapeHue << std::endl;
	 ++hueToAngleIterator;
 }

 void ExperimentHandler::setExperimentAsEnded()
 {
	 dnfcomposerHandler.stop();
 }

void ExperimentHandler::init()
{
	dnfcomposerHandler.init();
	experimentThread = std::thread(&ExperimentHandler::step, this);
}

//void ExperimentHandler::step()
//{
//	printExperimentSetupToConsole();
//	setExperimentSetupData();
//
//	for(int k = 0; k < hueToAngleMap.size(); k++)
//	{
//		setExpectedFieldBehaviour();
//
//		for(int i = 0; i < params.numberOfTrials; i++)
//		{
//			if (params.isDebugModeOn)
//				dnf_composer::log(dnf_composer::INFO, "Trial: " + std::to_string(i) + '\n');
//			dnfcomposerHandler.setTrial(i);
//
//			setupProcedure();
//			degenerationProcedure();
//			cleanUpTrial();
//			Sleep(20);
//		}
//		Sleep(50);
//	}
//	setExperimentAsEnded();
//}

void ExperimentHandler::setExperimentSetupData()
{
	//dnfcomposerHandler.setExperimentSetupData(params.degeneracyName, params.decisionTolerance, params.typeOfElementsDegenerated);
	dnfcomposerHandler.setNumberOfElementsToDegenerate(params.numberOfElementsToDegeneratePerIteration);
}

void ExperimentHandler::step()
{
	printExperimentSetupToConsole();
	setExperimentSetupData();

	for (int i = 0; i < params.numberOfTrials; i++)
	{
		params.currentTrial = i;
		if (params.isDebugModeOn)
		{
			std::string message = "Starting trial " + std::to_string(i) + " out of " + std::to_string(params.numberOfTrials) + ". ";
			message += "External stimulus: " + std::to_string(data.targetInputFieldCentroid) + ". ";
			message += "Expected input field centroid: " + std::to_string(data.targetInputFieldCentroid) + ". ";
			message += "Expected output field centroid: " + std::to_string(data.targetOutputFieldCentroid) + ".";
			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, message.c_str());
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

void ExperimentHandler::close()
{
	experimentThread.join();
	dnfcomposerHandler.close();
}

void ExperimentHandler::setupProcedure()
{
	// add and remove stimulus and wait for the fields to settle
	dnfcomposerHandler.setExternalInput(data.targetInputFieldCentroid);

	if (params.isDebugModeOn)
		dnf_composer::tools::logger::log(dnf_composer::tools::logger::INFO, "Added gaussian stimulus to perceptual field.\n");

	while (!dnfcomposerHandler.getHaveFieldsSettled());
	dnfcomposerHandler.setHaveFieldsSettled(false);
}

void ExperimentHandler::degenerationProcedure()
 {
	bool isOutputFieldDegenerated = hasOutputFieldDegenerated();

	while (!isOutputFieldDegenerated)
	{
		// save centroid of the output field
		//dnfcomposerHandler.setCentroidDataBeingAccessed(true);
		data.outputFieldCentroidHistory.push_back(dnfcomposerHandler.getOutputFieldCentroid());
		//dnfcomposerHandler.setCentroidDataBeingAccessed(false);
		//if (params.isDebugModeOn)
			//std::cout << "Output field centroid: " << dnfcomposerHandler.getOutputFieldCentroid() << std::endl;

		// apply degeneration and wait for the fields to settle
		dnfcomposerHandler.setDegeneracy(params.degeneracyType, params.fieldToDegenerate);
		if (params.isDebugModeOn)
		{
			std::string message = "Trial: " + std::to_string(params.currentTrial) + ". ";
			message += "Number of degenerated " + params.typeOfElementsDegenerated + ": "
				+ std::to_string(data.outputFieldCentroidHistory.size() * params.numberOfElementsToDegeneratePerIteration) + "/" + std::to_string(params.totalNumberOfElementsToDegenerate)
				+ " (" + std::to_string(static_cast<int>(static_cast<double>(data.outputFieldCentroidHistory.size() * params.numberOfElementsToDegeneratePerIteration) / params.totalNumberOfElementsToDegenerate * 100)) + "%%). ";

			// Use stringstream to format the doubles with 2 decimal places
			std::ostringstream stream;
			stream << std::fixed << std::setprecision(2);
			stream << "Perceptual field centroid is " << dnfcomposerHandler.getInputFieldCentroid() << " and should be " << data.targetInputFieldCentroid
				<< " (deviation of " << std::abs(dnfcomposerHandler.getInputFieldCentroid() - data.targetInputFieldCentroid) << "). ";
			stream << "Output field centroid is " << dnfcomposerHandler.getOutputFieldCentroid() << " and should be " << data.targetOutputFieldCentroid
				<< " (deviation of " << std::abs(dnfcomposerHandler.getOutputFieldCentroid() - data.targetOutputFieldCentroid) << ").\n";

			message += stream.str(); // Add the formatted string to the message
			log(dnf_composer::tools::logger::INFO, message);
		}

		while (!dnfcomposerHandler.getHaveFieldsSettled());

		dnfcomposerHandler.setHaveFieldsSettled(false);

		isOutputFieldDegenerated = hasOutputFieldDegenerated();
	}

	//if (params.isDebugModeOn)
		//log(dnf_composer::tools::logger::INFO, "Number of degeneration steps: " + std::to_string(data.outputFieldCentroidHistory.size()));
 }

void ExperimentHandler::cleanUpTrial()
{
	//dnf_composer::tools::logger::log(dnf_composer::tools::logger::INFO, "A trial has finished.\n");
	if (params.isDataSavingOn)
		saveOutputFieldCentroidToFile();
	Sleep(20);
	data.outputFieldCentroidHistory.clear();
	dnfcomposerHandler.closeSimulation();
}

bool ExperimentHandler::hasOutputFieldDegenerated()
{
	//dnfcomposerHandler.setCentroidDataBeingAccessed(true);
	//Sleep(4);
	const double outputFieldCentroid = dnfcomposerHandler.getOutputFieldCentroid();
	//dnfcomposerHandler.setCentroidDataBeingAccessed(false);

	if (outputFieldCentroid < 0)
		return true;

	return false;
}

void ExperimentHandler::saveOutputFieldCentroidToFile() const
{
	// Convert the double to a string with 1 decimal place
	std::ostringstream ss;
	ss << std::fixed << std::setprecision(1) << data.targetOutputFieldCentroid;
	const std::string decimalString = ss.str();

	const std::string filename = params.filePathPrefix + "results/" + decimalString + " "+ params.degeneracyName + " - centroids.txt";
	std::ofstream file(filename, std::ios::app); // Open the file in append mode

	if (!file.is_open())
	{
		if (params.isDebugModeOn)
		{
			const std::string message = "Failed to open the file for writing " + filename + '\n';
			dnf_composer::tools::logger::log(dnf_composer::tools::logger::FATAL, message);
		}
	}

	for (const auto& centroid : data.outputFieldCentroidHistory)
		file << centroid << " ";
	file << std::endl;

	file.close();

	if (params.isDebugModeOn)
	{
		const std::string message = "New centroids appended to " + filename + '\n';
		dnf_composer::tools::logger::log(dnf_composer::tools::logger::INFO, message);
	}
}

