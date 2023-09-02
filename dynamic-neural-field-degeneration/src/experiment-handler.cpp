
#include "../include/experiment-handler.h"

ExperimentHandler::ExperimentHandler(const ExperimentParameters& params)
	:params(params)
{
}

 void ExperimentHandler::printExperimentSetupToConsole() const
{
	std::cout << "Starting the experiment." << std::endl;
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

void ExperimentHandler::init()
{
	dnfcomposerHandler.init();
	experimentThread = std::thread(&ExperimentHandler::step, this);
}

void ExperimentHandler::step()
{
	printExperimentSetupToConsole();

	for(int i = 0; i < params.numberOfTrials; i++)
	{
		if constexpr (INFO)
			std::cout << "Trial: " << i << std::endl;

		setupProcedure();
		degenerationProcedure();
	}
	cleanUpTrial();
}

void ExperimentHandler::close()
{
	experimentThread.join();
	dnfcomposerHandler.close();
}


void ExperimentHandler::setupProcedure()
{
	dnfcomposerHandler.setInitializeFields();

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
		std::cout << "Output field centroid: " << dnfcomposerHandler.getOutputFieldCentroid() << std::endl;

		// apply degeneration and wait for the fields to settle
		dnfcomposerHandler.setDegeneracy(params.degeneracyType);

		while(!dnfcomposerHandler.getHaveFieldsSettled());
		dnfcomposerHandler.setHaveFieldsSettled(false);

		isOutputFieldDegenerated = hasOutputFieldDegenerated();
	}
 }

void ExperimentHandler::cleanUpTrial()
{
	if (params.isDataToBeSaved)
		saveOutputFieldCentroidToFile();
	data.outputFieldCentroidHistory.clear();
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
		std::cerr << "Failed to open the file for writing." << std::endl;

	for (const auto& centroid : data.outputFieldCentroidHistory)
		file << centroid << " ";
	file << std::endl;

	file.close();

	std::cout << "New centroids appended to " << filename << std::endl;
}

