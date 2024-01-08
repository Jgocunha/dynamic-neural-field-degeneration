#pragma once

#include "../lib/dynamic-neural-field-composer/include/user_interface/user_interface.h"


namespace dnf_composer
{
	class Simulation;
}

struct ExperimentWindowParameters
{
	int currentTrial = 0;
	std::string currentDegenerationType = "null";
	double maximumAllowedDeviation = 0.0;

	double perceptualFieldCentroid = 0.0, decisionFieldCentroid = 0.0;
	double expectedPerceptualFieldCentroid = 0.0, expectedDecisionFieldCentroid = 0.0;
	double perceptualFieldCentroidDeviation = 0.0, decisionFieldCentroidDeviation = 0.0;

	int numberOfDegeneratedElements = 0;
	std::string typeOfElementsDegenerated = "null";

	int relearningType = 0;
	int relearningEpochs = 0;
	double learningRate = 0.0;
	bool updateAllWeights = false;
	int maximumAmountOfDemonstrations = 0;
	int numOfRelearningCycles = 0;

};


class ExperimentWindow : public dnf_composer::user_interface::UserInterfaceWindow 
{
private:
	std::shared_ptr<dnf_composer::Simulation> simulation;
	ExperimentWindowParameters expWinParams;
public:
	ExperimentWindow(const std::shared_ptr<dnf_composer::Simulation>& simulation);
	void render() override;
	~ExperimentWindow() = default;


	void setExperimentSetupData(const std::string& currentDegenerationType,
		const double& maximumAllowedDeviation, const std::string& typeOfElementsDegenerated);
	void setCurrentTrial(const int& currentTrial);
	void setCentroids(const double& perceptualFieldCentroid, const double& decisionFieldCentroid);
	void setExpectedCentroids(const double& expectedPerceptualFieldCentroid, const double& expectedDecisionFieldCentroid);
	void setNumberOfDegeneratedElements(const int& numberOfDegeneratedElements);
	void setRelearningParameters(const int& relearningType, const int& relearningEpochs, const double& learningRate, const int& maximumRelearningCycles, const bool updateAllWeights);
	void setRelearningCycles(const int& numOfRelearningCycles);

private:

	static double calculateDeviation(const double& val1, const double& val2, const double& size);

	void setCurrentDegenerationType(const std::string& currentDegenerationType);
	void setMaximumAllowedDeviation(const double& maximumAllowedDeviation);
	void setTypeOfElementsDegenerated(const std::string& typeOfElementsDegenerated);

	void setPerceptualFieldCentroid(const double& perceptualFieldCentroid);
	void setDecisionFieldCentroid(const double& decisionFieldCentroid);

	void setExpectedPerceptualFieldCentroid(const double& expectedPerceptualFieldCentroid);
	void setExpectedDecisionFieldCentroid(const double& expectedDecisionFieldCentroid);

	void setPerceptualFieldCentroidDeviation();
	void setDecisionFieldCentroidDeviation();
private:
	void renderExperimentDetails() const;
	void renderFieldAnalysis() const;
	void renderDegenerationStatistics() const;
	void renderRelearningStatistics() const;
};