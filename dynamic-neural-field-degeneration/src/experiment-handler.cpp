
#include "../include/experiment-handler.h"

ExperimentHandler::ExperimentHandler(const ExperimentParameters& params)
	: dnfcomposerHandler(DnfcomposerHandler(params.isVisualisationOn)), params(params)
{
	data.outputFieldCentroidHistory.reserve(6000);
	std::advance(hueToAngleIterator, params.startingExternalStimulus);
	data.targetInputFieldCentroid = hueToAngleIterator->first;
	data.targetOutputFieldCentroid = hueToAngleIterator->second;
}

 void ExperimentHandler::printExperimentSetupToConsole() const
{
	 std::ostringstream logStream;

	logStream << "Starting the experiment." << std::endl;
	logStream << "----------------------------------------" << std::endl;
	logStream << "Data saving is " << (params.isDataSavingOn ? "on" : "off") << std::endl;
	logStream << "Debug mode is " << (params.isDebugModeOn ? "on" : "off") << std::endl;
	logStream << "Visualization is " << (params.isVisualisationOn ? "on" : "off") << std::endl;
	logStream << "Number of trials: " << params.numberOfTrials << std::endl;
	logStream << "Decision tolerance: " << params.decisionTolerance << std::endl;
	logStream << "Degeneracy type: " << params.degeneracyName << std::endl;
	logStream << "Number of elements to degenerate: " << params.numberOfElementsToDegenerate << std::endl;
	logStream << "Initial percentage of degeneration: " << params.initialPercentageOfDegeneration << std::endl;
	logStream << "Target percentage of degeneration: " << params.targetPercentageOfDegeneration << std::endl;
	logStream << "----------------------------------------" << std::endl;

	 log(dnf_composer::LogLevel::INFO, logStream.str());
	//if (params.isDebugModeOn)
	//{
	//	const std::string message = "External stimulus: " + std::to_string(data.targetOutputFieldCentroid) + '\n';
	//	dnf_composer::log(dnf_composer::LogLevel::INFO, message);
	//}
}

 void ExperimentHandler::setExperimentSetupData()
 {
	dnfcomposerHandler.setExperimentSetupData(params.degeneracyName, params.decisionTolerance, params.typeOfElementsDegenerated);
	dnfcomposerHandler.setNumberOfElementsToDegenerate(params.numberOfElementsToDegenerate);
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

	 dnfcomposerHandler.setExpectedFieldBehavior(data.targetInputFieldCentroid, data.targetOutputFieldCentroid);
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

void ExperimentHandler::step()
{
	printExperimentSetupToConsole();
	setExperimentSetupData();

	for (int i = 0; i < params.numberOfTrials; i++)
	{
		if (params.isDebugModeOn)
			dnf_composer::log(dnf_composer::INFO, "Trial: " + std::to_string(i) + '\n');
		dnfcomposerHandler.setTrial(i);

		for (int k = 0; k < static_cast<int>(hueToAngleMap.size()); k++)
		{
			setExpectedFieldBehaviour();
			setupProcedure();
			Sleep(1000);
			//degenerationProcedure();
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
		dnf_composer::log(dnf_composer::INFO, "Added gaussian stimulus to perceptual field.\n");

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
			dnf_composer::log(dnf_composer::INFO, "Degeneration step number: " + std::to_string(data.outputFieldCentroidHistory.size()) + ".\n");

		while (!dnfcomposerHandler.getHaveFieldsSettled());

		dnfcomposerHandler.setHaveFieldsSettled(false);

		isOutputFieldDegenerated = hasOutputFieldDegenerated();
	}

	if (params.isDebugModeOn)
		log(dnf_composer::LogLevel::INFO, "Number of degeneration steps: " + std::to_string(data.outputFieldCentroidHistory.size()) + '\n');
 }

void ExperimentHandler::cleanUpTrial()
{
	dnf_composer::log(dnf_composer::INFO, "A trial has finished.\n");
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
			dnf_composer::log(dnf_composer::LogLevel::FATAL, message);
		}
	}

	for (const auto& centroid : data.outputFieldCentroidHistory)
		file << centroid << " ";
	file << std::endl;

	file.close();

	if (params.isDebugModeOn)
	{
		const std::string message = "New centroids appended to " + filename + '\n';
		dnf_composer::log(dnf_composer::LogLevel::INFO, message);
	}
}

