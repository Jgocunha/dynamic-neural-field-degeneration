
#include "../include/experiment-handler.h"

ExperimentHandler::ExperimentHandler(const ExperimentParameters& params)
	: dnfcomposerHandler(DnfcomposerHandler(params.isVisualisationOn)), params(params)
{
	data.outputFieldCentroidHistory.reserve(6000);
	std::advance(hueToAngleIterator, params.startingExternalStimulus);
	data.targetInputFieldCentroid = hueToAngleIterator->first;
	data.targetOutputFieldCentroid = hueToAngleIterator->second;

	if (params.isDebugModeOn)
		std::cout << "External stimulus: " << data.targetOutputFieldCentroid << std::endl;
}

 void ExperimentHandler::printExperimentSetupToConsole() const
{
	std::cout << "Starting the experiment." << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Data saving is " << (params.isDataSavingOn ? "on" : "off") << std::endl;
	std::cout << "Debug mode is " << (params.isDebugModeOn ? "on" : "off") << std::endl;
	std::cout << "Visualization is " << (params.isVisualisationOn ? "on" : "off") << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Number of trials: " << params.numberOfTrials << std::endl;
	std::cout << "Decision tolerance: " << params.decisionTolerance << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Degeneracy type: " << params.degeneracyName << std::endl;
	std::cout << "Initial percentage of degeneration: " << params.initialPercentageOfDegeneration << std::endl;
	std::cout << "Target percentage of degeneration: " << params.targetPercentageOfDegeneration << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "----------------------------------------" << std::endl << std::endl;
}

 void ExperimentHandler::setExperimentSetupData() const
 {
	 dnfcomposerHandler.setExperimentSetupData(params.degeneracyName, params.decisionTolerance, params.typeOfElementsDegenerated);
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

void ExperimentHandler::step()
{
	printExperimentSetupToConsole();
	setExperimentSetupData();

	for(int k = 0; k < hueToAngleMap.size(); k++)
	{
		setExpectedFieldBehaviour();

		for(int i = 0; i < params.numberOfTrials; i++)
		{
			if (params.isDebugModeOn)
				std::cout << "Trial: " << i << std::endl;
			dnfcomposerHandler.setTrial(i);

			setupProcedure();
			//Sleep(20);
			degenerationProcedure();
			cleanUpTrial();
		}
		Sleep(20);
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

	while (!dnfcomposerHandler.getHaveFieldsSettled());
	dnfcomposerHandler.setHaveFieldsSettled(false);
}

void ExperimentHandler::degenerationProcedure()
 {
	bool isOutputFieldDegenerated = hasOutputFieldDegenerated();

	while (!isOutputFieldDegenerated)
	{
		// save centroid of the output field
		Sleep(2);
		dnfcomposerHandler.setCentroidDataBeingAccessed(true);
		Sleep(2);
		data.outputFieldCentroidHistory.push_back(dnfcomposerHandler.getOutputFieldCentroid());
		Sleep(2);
		dnfcomposerHandler.setCentroidDataBeingAccessed(false);
		Sleep(2);
		//if (params.isDebugModeOn)
			//std::cout << "Output field centroid: " << dnfcomposerHandler.getOutputFieldCentroid() << std::endl;

		// apply degeneration and wait for the fields to settle
		dnfcomposerHandler.setDegeneracy(params.degeneracyType, params.fieldToDegenerate);

		while (!dnfcomposerHandler.getHaveFieldsSettled());

		dnfcomposerHandler.setHaveFieldsSettled(false);

		isOutputFieldDegenerated = hasOutputFieldDegenerated();
	}

	std::cout << "Number of degeneration steps: " << data.outputFieldCentroidHistory.size() << std::endl;
 }

void ExperimentHandler::cleanUpTrial()
{
	if (params.isDataSavingOn)
		saveOutputFieldCentroidToFile();
	Sleep(20);
	data.outputFieldCentroidHistory.clear();
	dnfcomposerHandler.closeSimulation();
}

bool ExperimentHandler::hasOutputFieldDegenerated()
{
	dnfcomposerHandler.setCentroidDataBeingAccessed(true);
	Sleep(4);
	const double outputFieldCentroid = dnfcomposerHandler.getOutputFieldCentroid();
	dnfcomposerHandler.setCentroidDataBeingAccessed(false);
	Sleep(4);

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
		if (params.isDebugModeOn)
			std::cerr << "Failed to open the file for writing." << std::endl;

	for (const auto& centroid : data.outputFieldCentroidHistory)
		file << centroid << " ";
	file << std::endl;

	file.close();

	if (params.isDebugModeOn)
		std::cout << "New centroids appended to " << filename << std::endl;
}

