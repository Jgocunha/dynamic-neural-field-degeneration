#pragma once

#include <string>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <tools/logger.h>

#include "degenerate_element_parameters.h"


struct ExperimentParameters
{
	std::string filePathPrefix = "../../../data/";

	double decisionTolerance = 5;
	int numberOfTrials = 10;

	ElementDegeneracyType degeneracyType = ElementDegeneracyType::WEIGHTS_DEACTIVATE;
	std::string degeneracyName = "deactivate-weights";
	std::string typeOfElementsDegenerated = "weights";
	std::string fieldToDegenerate = "perceptual";

	int startingExternalStimulus = 0;
	int initialPercentageOfDegeneration = 0;
	int targetPercentageOfDegeneration = 100;
	int currentPercentageOfDegeneration = 0;
	int numberOfElementsToDegeneratePerIteration = 0;
	int totalNumberOfElementsToDegenerate = 0;
	int currentTrial = 0;

	bool isDataSavingOn = false;
	bool isVisualisationOn = true;
	bool isDebugModeOn = true;

	ExperimentParameters();
	bool readUserDefinedParameters();
	void setAutomaticallyDefinedParameters();
	void print();
};