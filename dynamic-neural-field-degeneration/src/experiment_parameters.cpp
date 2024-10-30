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
        relearningParameters.print();
    }


    std::string ExperimentParameters::getSavePath() const
	{
        std::string filename = std::string(OUTPUT_DIRECTORY) + "/results/";

        filename = filename + degenerationParameters.name;

        switch (relearningParameters.type)
        {
        case relearning::RelearningType::ONLY_DEGENERATED_CASES:
            filename = filename + " Only-degenerated-cases ";
            break;
        case relearning::RelearningType::ALL_CASES:
            filename = filename + "All-cases ";
            break;
        }

        filename = filename + " Epochs-" + std::to_string(relearningParameters.numberOfEpochs);
        filename = filename + " MaxCycles-" + std::to_string(relearningParameters.maxAmountOfDemonstrations);
        filename = filename + " Update-all-weights-" + std::to_string(relearningParameters.updateAllWeights);
        filename = filename + ".txt";

        return filename;
	}
}