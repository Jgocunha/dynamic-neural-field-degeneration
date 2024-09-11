#include "experiment_parameters.h"

namespace experiment
{
	ExperimentParameters::ExperimentParameters()
	{
		read();
	}

    void ExperimentParameters::read()
    {
        std::ifstream file(std::string(PROJECT_DIR) + "/experiment_parameters.json");

        if (!file.is_open())
            std::cerr << "Error: Could not open the JSON parameters file." << std::endl;

        nlohmann::json jsonData;
        file >> jsonData;

        auto experimentParams = jsonData.at("experiment_parameters");
        numberOfTrials = experimentParams.at("numberOfTrials").get<int>();
        startingExternalStimulus = experimentParams.at("startingExternalStimulus").get<int>();
        decisionTolerance = experimentParams.at("decisionTolerance").get<double>();
        isDataSavingOn = experimentParams.at("isDataSavingOn").get<bool>();
        isVisualizationOn = experimentParams.at("isVisualizationOn").get<bool>();
        isDebugModeOn = experimentParams.at("isDebugModeOn").get<bool>();
    }

	std::string ExperimentParameters::toString() const
	{
        std::ostringstream logStream;
        logStream << "Experiment parameters" << std::endl;
        logStream << "----------------------------------------" << std::endl;
        logStream << "Data saving is " << (isDataSavingOn ? "on" : "off") << std::endl;
        logStream << "Debug mode is " << (isDebugModeOn ? "on" : "off") << std::endl;
        logStream << "Visualization is " << (isVisualizationOn ? "on" : "off") << std::endl;
        logStream << "Number of trials: " << numberOfTrials << std::endl;
        logStream << "Decision tolerance: " << decisionTolerance << std::endl;
        logStream << "----------------------------------------" << std::endl;
        return logStream.str();
	}

    void ExperimentParameters::print() const
    {
        dnf_composer::tools::logger::log(dnf_composer::tools::logger::LogLevel::INFO, toString());
        degenerationParameters.print();
    }
}