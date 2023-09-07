
#include "../include/experiment-handler.h"

ExperimentHandler::ExperimentHandler(const ExperimentParameters& params)
	: dnfcomposerHandler(DnfcomposerHandler(params.isVisualisationOn)), params(params)
{
	data.outputFieldCentroidHistory.reserve(6000);
}

 void ExperimentHandler::printExperimentSetupToConsole() const
{
	std::cout << "Starting the experiment." << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Data saving is " << (params.isDataSavingOn ? "on" : "off") << std::endl;
	std::cout << "Debug mode is " << (params.isDebugModeOn ? "on" : "off") << std::endl;
	std::cout << "Visualisation is " << (params.isVisualisationOn ? "on" : "off") << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Number of trials: " << params.numberOfTrials << std::endl;
	std::cout << "Decision tolerance: " << params.decisionTolerance << std::endl;
	std::cout << "Target external stimulus position: " << params.targetExternalStimulusPosition << std::endl;
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

 void ExperimentHandler::setExpectedFieldBehaviour() const
 {
	 dnfcomposerHandler.setExpectedFieldBehavior(params.targetExternalStimulusPosition, params.targetOutputCentroid);
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

// debug step
//void ExperimentHandler::step()
//{
//	printExperimentSetupToConsole();
//	setExperimentSetupData();
//	setExpectedFieldBehaviour();
//
//	setupProcedure(0);
//
//	for (int i = 0; i < 648; i++)
//	{
//		if (params.isDebugModeOn)
//			std::cout << "Trial: " << i << std::endl;
//
//		degenerationProcedure();
//		//cleanUpTrial();
//	}
//
//	std::cout << "Experiment ended." << std::endl;
//	setExperimentAsEnded();
//}

void ExperimentHandler::step()
{
	printExperimentSetupToConsole();
	setExperimentSetupData();
	setExpectedFieldBehaviour();

	for(int i = 0; i < params.numberOfTrials; i++)
	{
		if (params.isDebugModeOn)
			std::cout << "Trial: " << i << std::endl;

		setupProcedure(i);
		Sleep(20);
		degenerationProcedure();
		cleanUpTrial();
	}

	setExperimentAsEnded();
}

// debug degeneration
//void ExperimentHandler::degenerationProcedure()
//{
//	bool isOutputFieldDegenerated = hasOutputFieldDegenerated();
//
//	//while (1)
//	{
//		// save centroid of the output field
//		data.outputFieldCentroidHistory.push_back(dnfcomposerHandler.getOutputFieldCentroid());
//		//if (params.isDebugModeOn)
//			//std::cout << "Output field centroid: " << dnfcomposerHandler.getOutputFieldCentroid() << std::endl;
//
//		// apply degeneration and wait for the fields to settle
//		dnfcomposerHandler.setDegeneracy(params.degeneracyType, params.fieldToDegenerate);
//
//		while (!dnfcomposerHandler.getHaveFieldsSettled());
//
//		dnfcomposerHandler.setHaveFieldsSettled(false);
//
//		isOutputFieldDegenerated = hasOutputFieldDegenerated();
//	}
//}

void ExperimentHandler::close()
{
	experimentThread.join();
	dnfcomposerHandler.close();
}

void ExperimentHandler::setupProcedure(const int& trial)
{
	dnfcomposerHandler.setTrial(trial);

	// add and remove stimulus and wait for the fields to settle
	dnfcomposerHandler.setExternalInput(params.targetExternalStimulusPosition);

	while (!dnfcomposerHandler.getHaveFieldsSettled());
	dnfcomposerHandler.setHaveFieldsSettled(false);
}

void ExperimentHandler::degenerationProcedure()
 {
	bool isOutputFieldDegenerated = hasOutputFieldDegenerated();

	while (!isOutputFieldDegenerated)
	{
		// save centroid of the output field
		data.outputFieldCentroidHistory.push_back(dnfcomposerHandler.getOutputFieldCentroid());
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
	data.outputFieldCentroidHistory.clear();

	dnfcomposerHandler.closeSimulation();
}

bool ExperimentHandler::hasOutputFieldDegenerated() const
{
	const double outputFieldCentroid = dnfcomposerHandler.getOutputFieldCentroid();

	if (outputFieldCentroid < 0)
		return true;

	return false;
}

void ExperimentHandler::saveOutputFieldCentroidToFile() const
{
	const std::string filename = params.filePathPrefix + params.degeneracyName + " - centroids.txt";
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

