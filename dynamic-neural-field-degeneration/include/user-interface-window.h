#pragma once

#include "../lib/dynamic-neural-field-composer/include/user_interface/user_interface.h"


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
};


class ExperimentWindow : public UserInterfaceWindow 
{
private:
	std::shared_ptr<Simulation> simulation;
	ExperimentWindowParameters expWinParams;

public:
	ExperimentWindow(const std::shared_ptr<Simulation>& simulation);
	void render() override;
	~ExperimentWindow() = default;

	void setExperimentSetupData(const std::string& currentDegenerationType, 
		const double& maximumAllowedDeviation, const std::string& typeOfElementsDegenerated);
	void setCurrentTrial(const int& currentTrial);
	void setCentroids(const double& perceptualFieldCentroid, const double& decisionFieldCentroid);
	void setExpectedCentroids(const double& expectedPerceptualFieldCentroid, const double& expectedDecisionFieldCentroid);
	void setNumberOfDegeneratedElements(const int& numberOfDegeneratedElements);

private:
	void setCurrentDegenerationType(const std::string& currentDegenerationType);
	void setMaximumAllowedDeviation(const double& maximumAllowedDeviation);
	void setTypeOfElementsDegenerated(const std::string& typeOfElementsDegenerated);

	void setPerceptualFieldCentroid(const double& perceptualFieldCentroid);
	void setDecisionFieldCentroid(const double& decisionFieldCentroid);

	void setExpectedPerceptualFieldCentroid(const double& expectedPerceptualFieldCentroid);
	void setExpectedDecisionFieldCentroid(const double& expectedDecisionFieldCentroid);

	void setPerceptualFieldCentroidDeviation(const double& perceptualFieldCentroidDeviation);
	void setDecisionFieldCentroidDeviation(const double& decisionFieldCentroidDeviation);

private:
	void renderExperimentDetails() const;
	void renderFieldAnalysis() const;
	void renderDegenerationStatistics() const;

public:
	static double calculateFieldCentroidDeviation(const double &fieldCentroid, const double &expectedFieldCentroid);
};