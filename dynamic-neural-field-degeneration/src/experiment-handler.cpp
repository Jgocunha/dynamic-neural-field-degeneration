
#include "../include/experiment-handler.h"

ExperimentHandler::ExperimentHandler(const ExperimentParameters& params)
	: dnfcomposerHandler(DnfcomposerHandler(params.isVisualizationOn)), params(params)
{
	this->params.setOtherDegeneracyParameters();
	printExperimentSetupToConsole();
	setExperimentSetupData();
}

 void ExperimentHandler::printExperimentSetupToConsole() const
{
	std::cout << "Starting the experiment." << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Data saving is " << (params.isDataSavingOn ? "on" : "off") << std::endl;
	std::cout << "Debug mode is " << (params.isDebugModeOn ? "on" : "off") << std::endl;
	std::cout << "Visualization is " << (params.isVisualizationOn ? "on" : "off") << std::endl;
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

 void ExperimentHandler::setExpectedFieldBehaviour() const
 {
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

	for(int i = 0; i < params.numberOfTrials; i++)
	{
		if (params.isDebugModeOn)
			std::cout << "Trial: " << i << std::endl;
		dnfcomposerHandler.setTrial(i);

		do
		{
			stats.decisionHistory.clear();
			for (int j = 0; j < params.numberOfStimulusPerTrial; j++)
			{
				addExternalStimulus();
				readTargetAngle();
				verifyDecisionMaking();
			}

			//if(!hasOutputFieldDegenerated())
			degenerationProcedure();
		} while (!hasOutputFieldDegenerated());

		cleanUpTrial();
	}

	setExperimentAsEnded();
}

void ExperimentHandler::close()
{
	experimentThread.join();
	dnfcomposerHandler.close();
}

void ExperimentHandler::addExternalStimulus()
{
	if (hueToAngleIterator == hueToAngleMap.end())
		hueToAngleIterator = hueToAngleMap.begin();

	data.targetInputFieldCentroid = hueToAngleIterator->first;
	//if (params.isDebugModeOn)
		//std::cout << "External stimulus: " << data.shapeHue << std::endl;
	++hueToAngleIterator;

	dnfcomposerHandler.setExternalInput(data.targetInputFieldCentroid);

	// wait for the shape hue to be read
	while (!dnfcomposerHandler.getHaveFieldsSettled());
	dnfcomposerHandler.setHaveFieldsSettled(false);
}

void ExperimentHandler::readTargetAngle()
{
	data.outputFieldCentroid = dnfcomposerHandler.getOutputFieldCentroid();
	// save centroid here
	getExpectedTargetAngle();
}

void ExperimentHandler::getExpectedTargetAngle()
{
	// Check if cuboidHue exists in the map
	auto closestHueIter = hueToAngleMap.end();
	double minDifference = params.decisionTolerance;

	for (auto it = hueToAngleMap.begin(); it != hueToAngleMap.end(); ++it)
	{
		double difference = std::abs(data.targetInputFieldCentroid - it->first);
		if (difference <= params.decisionTolerance && difference < minDifference)
		{
						minDifference = difference;
			closestHueIter = it;
		}
	}

	if (closestHueIter != hueToAngleMap.end())
		data.targetOutputFieldCentroid = closestHueIter->second;
}

void ExperimentHandler::verifyDecisionMaking()
{
	// 0 - correct decision
	// 1 - incorrect decision
	// 2 - no decision

	stats.decision = static_cast<ExperimentStatistics::DecisionClassification>(
		!(std::abs(data.targetOutputFieldCentroid - data.outputFieldCentroid) <= params.decisionTolerance)); // 0 or 1

	if (data.outputFieldCentroid < 0)
		stats.decision = ExperimentStatistics::DecisionClassification::NO_DECISION;

	//std::cout << "Decision: " << static_cast<int>(isCorrectDecision) << std::endl;
	stats.decisionHistory.push_back(stats.decision);
}

bool ExperimentHandler::hasOutputFieldDegenerated() const
{
	if(params.isDebugModeOn)
	{
		std::cout << "Decision history ";
		for (auto decision : stats.decisionHistory)
			std::cout << static_cast<int>(decision);
		std::cout << std::endl;
	}

	// if the sum of decisionHistory is equal to 2*size(), then the output field has degenerated
	// 2 2 2 2 2 2 2 = 14
	int sum = 0;
	for (auto decision : stats.decisionHistory)
		sum += static_cast<int>(decision);
	if (sum == 2*stats.decisionHistory.size())
		return true;
	return false;
}

void ExperimentHandler::degenerationProcedure()
 {
	// save centroid of the output field
	//data.outputFieldCentroidHistory.push_back(dnfcomposerHandler.getOutputFieldCentroid());

	//if (params.isDebugModeOn)
		//std::cout << "Output field centroid: " << dnfcomposerHandler.getOutputFieldCentroid() << std::endl;

	static int degenerationStep = 0;
	degenerationStep++;

	// apply degeneration and wait for the fields to settle
	dnfcomposerHandler.setDegeneracy(params.degeneracyType, params.fieldToDegenerate);

	while (!dnfcomposerHandler.getHaveFieldsSettled());

	dnfcomposerHandler.setHaveFieldsSettled(false);

	if(params.isDebugModeOn)
		std::cout << "Number of degeneration steps: " << degenerationStep << std::endl;
 }

void ExperimentHandler::cleanUpTrial()
{
	//if (params.isDataSavingOn)
		//saveOutputFieldCentroidToFile();
	data.outputFieldCentroidHistory.clear();

	dnfcomposerHandler.closeSimulation();
} 

//bool ExperimentHandler::hasOutputFieldDegenerated() const
//{
//	const double outputFieldCentroid = dnfcomposerHandler.getOutputFieldCentroid();
//
//	if (outputFieldCentroid < 0)
//		return true;
//
//	return false;
//}

//void ExperimentHandler::saveOutputFieldCentroidToFile() const
//{
//	const std::string filename = params.filePathPrefix + params.degeneracyName + " - centroids.txt";
//	std::ofstream file(filename, std::ios::app); // Open the file in append mode
//
//	if (!file.is_open())
//		if (params.isDebugModeOn)
//			std::cerr << "Failed to open the file for writing." << std::endl;
//
//	for (const auto& centroid : data.outputFieldCentroidHistory)
//		file << centroid << " ";
//	file << std::endl;
//
//	file.close();
//
//	if (params.isDebugModeOn)
//		std::cout << "New centroids appended to " << filename << std::endl;
//}

