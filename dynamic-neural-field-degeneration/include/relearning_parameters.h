#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <tools/logger.h>

#include "degeneration_parameters.h"

namespace experiment
{
	namespace relearning
	{
		enum class RelearningType
		{
			ALL_CASES = 0,
			ONLY_DEGENERATED_CASES
		};

		struct RelearningParameters
		{
			RelearningType type;
			double learningRate;
			int numberOfEpochs;
			int maxAmountOfDemonstrations;
			bool updateAllWeights;
			int targetRelearningPositions = 0;

			RelearningParameters();
			void read();
			std::string toString() const;
			void print() const;
		};
	}
}