#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <tools/logger.h>

#include "degeneration_parameters.h"

namespace experiment
{
	struct ExperimentParameters
	{
		std::string id = "";
		int numberOfTrials;
		int startingExternalStimulus;
		int currentTrial = 0;
		double decisionTolerance;
		bool isDataSavingOn;
		bool isVisualizationOn;
		bool isDebugModeOn;

		degeneration::DegenerationParameters degenerationParameters;

		ExperimentParameters();
		void read();
		std::string toString() const;
		void print() const;
	};
}
