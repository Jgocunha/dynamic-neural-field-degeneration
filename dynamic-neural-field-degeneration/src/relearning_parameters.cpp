#include "relearning_parameters.h"

namespace experiment
{
	namespace relearning
	{
		RelearningParameters::RelearningParameters()
		{
			read();
		}

		void RelearningParameters::read()
		{
			std::ifstream file(std::string(PROJECT_DIR) + "/experiment_parameters.json");

			if (!file.is_open())
				std::cerr << "Error: Could not open the JSON parameters file." << std::endl;

			nlohmann::json jsonData;
			file >> jsonData;

			auto relearningParameters = jsonData.at("relearning_parameters");
			const std::string experimentTypeStr = relearningParameters.at("relearningType").get<std::string>();
			if (experimentTypeStr == "ALL_CASES") {
				type = RelearningType::ALL_CASES;
			}
			else if (experimentTypeStr == "ONLY_DEGENERATED_CASES") {
				type = RelearningType::ONLY_DEGENERATED_CASES;
			}

			learningRate = relearningParameters.at("learningRate").get<double>();
			numberOfEpochs = relearningParameters.at("numberOfEpochs").get<int>();
			maxAmountOfDemonstrations = relearningParameters.at("maxAmountOfDemonstrations").get<int>();
			updateAllWeights = relearningParameters.at("updateAllWeights").get<bool>();
		}

		std::string RelearningParameters::toString() const
		{
			std::ostringstream logStream;
			logStream << "Relearning parameters" << std::endl;
			logStream << "----------------------------------------" << std::endl;
			logStream << "Type: " << (type == RelearningType::ALL_CASES ? "ALL_CASES" : "ONLY_DEGENERATED_CASES") << std::endl;
			logStream << "Learning rate: " << learningRate << std::endl;
			logStream << "Number of epochs: " << numberOfEpochs << std::endl;
			logStream << "Max amount of demonstrations: " << maxAmountOfDemonstrations << std::endl;
			logStream << "Update all weights: " << (updateAllWeights ? "true" : "false") << std::endl;
			logStream << "----------------------------------------" << std::endl;
			return logStream.str();
		}

		void RelearningParameters::print() const
		{
			dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, toString());
		}
	}
}
