#include "degeneration_parameters.h"

namespace experiment
{
	namespace degeneration
	{
		DegenerationParameters::DegenerationParameters()
		{
			read();
			setIdentifiersFromType();
		}

		void DegenerationParameters::read()
		{
			std::ifstream file(std::string(PROJECT_DIR) + "/experiment_parameters.json");

			if (!file.is_open())
				std::cerr << "Error: Could not open the JSON parameters file." << std::endl;

			nlohmann::json jsonData;
			file >> jsonData;

			auto degenerationParams = jsonData.at("degeneration_parameters");
			const std::string experimentTypeStr = degenerationParams.at("experimentType").get<std::string>();
			if (experimentTypeStr == "WEIGHTS_DEACTIVATE") {
				type = ElementDegeneracyType::WEIGHTS_DEACTIVATE;
			}
			else if (experimentTypeStr == "NEURONS_DEACTIVATE") {
				type = ElementDegeneracyType::NEURONS_DEACTIVATE;
			}
			else if (experimentTypeStr == "WEIGHTS_RANDOMIZE") {
				type = ElementDegeneracyType::WEIGHTS_RANDOMIZE;
			}
			else if (experimentTypeStr == "WEIGHTS_REDUCE") {
				type = ElementDegeneracyType::WEIGHTS_REDUCE;
			}
			else {
				type = ElementDegeneracyType::NONE;
			}

			field = degenerationParams.at("fieldToDegenerate").get<std::string>();
			initialPercentage = degenerationParams.at("initialPercentageOfDegeneration").get<int>();
			targetPercentage = degenerationParams.at("targetPercentageOfDegeneration").get<int>();
			numberOfElementsToDegeneratePerIteration = degenerationParams.at("numberOfElementsToDegeneratePerIteration").get<int>();
			totalNumberOfElementsToDegenerate = degenerationParams.at("totalNumberOfElementsToDegenerate").get<int>();
			incrementOfDegenerationInPercentage = degenerationParams.at("incrementOfDegenerationInPercentage").get<double>();
		}

		std::string DegenerationParameters::toString() const
		{
			std::ostringstream logStream;
			logStream << "Degeneration parameters" << std::endl;
			logStream << "----------------------------------------" << std::endl;
			logStream << "Name: " << name << std::endl;
			logStream << "Type of elements to degenerate: " << typeOfElement << std::endl;
			logStream << "Field to degenerate: " << field << std::endl;
			logStream << "Initial percentage of degeneration: " << initialPercentage << std::endl;
			logStream << "Target percentage of degeneration: " << targetPercentage << std::endl;
			logStream << "Number of elements to degenerate per iteration: " << numberOfElementsToDegeneratePerIteration << std::endl;
			logStream << "Total number of elements to degenerate: " << totalNumberOfElementsToDegenerate << std::endl;
			logStream << "Increment of degeneration in percentage: " << incrementOfDegenerationInPercentage << std::endl;
			logStream << "----------------------------------------" << std::endl;
			return logStream.str();
		}

		void DegenerationParameters::print() const
		{
			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, toString());
		}

		void DegenerationParameters::setIdentifiersFromType()
		{
			switch (type)
			{
			case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
				name = "deactivate";
				typeOfElement = "weights";
				break;
			case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
				name = "randomize";
				typeOfElement = "weights";
				break;
			case ElementDegeneracyType::WEIGHTS_REDUCE:
				name = "reduce 0.005";
				typeOfElement = "weights";
				break;
			case ElementDegeneracyType::NEURONS_DEACTIVATE:
				if (field == "perceptual")
					typeOfElement = "pre-synaptic neurons";
				else //if (field == "output")
					typeOfElement = "post-synaptic neurons";
				name = "deactivate";
				break;
			case ElementDegeneracyType::NONE:
				break;
			}
			name = name + " " + typeOfElement;
		}

	}
}
