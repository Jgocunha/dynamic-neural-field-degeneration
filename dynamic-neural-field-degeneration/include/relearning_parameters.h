#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

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
			int targetRelearningPositions;
			bool updateAllWeights;

			RelearningParameters();
			void read();
			std::string toString() const;
			void print() const;
		};
	}
}