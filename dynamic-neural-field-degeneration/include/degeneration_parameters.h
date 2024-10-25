#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <tools/logger.h>
#include <fstream>
#include <iostream>
#include <tools/logger.h>

namespace experiment
{
	namespace degeneration
	{
		enum class ElementDegeneracyType
		{
			NONE = 0,
			NEURONS_DEACTIVATE,
			WEIGHTS_DEACTIVATE,
			WEIGHTS_RANDOMIZE,
			WEIGHTS_REDUCE,
		};

		struct DegenerationParameters
		{
			ElementDegeneracyType type;
			std::string name;
			std::string typeOfElement;
			std::string field;

			int initialPercentage;
			int targetPercentage;
			int currentPercentage = 0;
			int numberOfElementsToDegeneratePerIteration;
			int totalNumberOfElementsToDegenerate;
			double incrementOfDegenerationInPercentage;

			DegenerationParameters();
			void read();
			std::string toString() const;
			void print() const;
			void setIdentifiersFromType();
		};
	}
}