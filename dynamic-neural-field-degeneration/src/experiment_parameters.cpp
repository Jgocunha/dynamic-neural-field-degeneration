#include "experiment_parameters.h"


ExperimentParameters::ExperimentParameters()
{
	if (!readUserDefinedParameters())
	{
		throw std::runtime_error("Could not read experiment parameters from file.");
	}
	setAutomaticallyDefinedParameters();
}

bool ExperimentParameters::readUserDefinedParameters()
{
    std::ifstream file(std::string(PROJECT_DIR) + "/experiment_parameters.json");

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the JSON parameters file." << std::endl;
        return false;
    }

    nlohmann::json jsonData;
    file >> jsonData;

    try {
        filePathPrefix = jsonData.at("filePathPrefix").get<std::string>();
        decisionTolerance = jsonData.at("decisionTolerance").get<double>();
        numberOfTrials = jsonData.at("numberOfTrials").get<int>();

        std::string experimentTypeStr = jsonData.at("experimentType").get<std::string>();
        if (experimentTypeStr == "WEIGHTS_DEACTIVATE") {
            degeneracyType = ElementDegeneracyType::WEIGHTS_DEACTIVATE;
        }
        else if (experimentTypeStr == "NEURONS_DEACTIVATE") {
            degeneracyType = ElementDegeneracyType::NEURONS_DEACTIVATE;
        }
        else if (experimentTypeStr == "WEIGHTS_RANDOMIZE") {
            degeneracyType = ElementDegeneracyType::WEIGHTS_RANDOMIZE;
        }
        else if (experimentTypeStr == "WEIGHTS_REDUCE") {
            degeneracyType = ElementDegeneracyType::WEIGHTS_REDUCE;
        }
        else {
            degeneracyType = ElementDegeneracyType::NONE;
        }

        fieldToDegenerate = jsonData.at("fieldToDegenerate").get<std::string>();
        startingExternalStimulus = jsonData.at("startingExternalStimulus").get<int>();
        initialPercentageOfDegeneration = jsonData.at("initialPercentageOfDegeneration").get<int>();
        targetPercentageOfDegeneration = jsonData.at("targetPercentageOfDegeneration").get<int>();
        currentPercentageOfDegeneration = jsonData.at("currentPercentageOfDegeneration").get<int>();
        numberOfElementsToDegeneratePerIteration = jsonData.at("numberOfElementsToDegeneratePerIteration").get<int>();
        totalNumberOfElementsToDegenerate = jsonData.at("totalNumberOfElementsToDegenerate").get<int>();

        isDataSavingOn = jsonData.at("isDataSavingOn").get<bool>();
        isVisualisationOn = jsonData.at("isVisualisationOn").get<bool>();
        isDebugModeOn = jsonData.at("isDebugModeOn").get<bool>();
    }
    catch (nlohmann::json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return false;
    }

    file.close();
    return true;
}

void ExperimentParameters::setAutomaticallyDefinedParameters()
{
    switch (degeneracyType)
    {
    case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
        degeneracyName = "deactivate";
        typeOfElementsDegenerated = "weights";
        //numberOfElementsToDegeneratePerIteration = 1000;
        //totalNumberOfElementsToDegenerate = 720 * 360;
        break;
    case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
        degeneracyName = "randomize";
        typeOfElementsDegenerated = "weights";
        //numberOfElementsToDegeneratePerIteration = 1000;
        //totalNumberOfElementsToDegenerate = 720 * 360;
        break;
    case ElementDegeneracyType::WEIGHTS_REDUCE:
        degeneracyName = "reduce 0.005";
        typeOfElementsDegenerated = "weights";
        //numberOfElementsToDegeneratePerIteration = 1000;
        //totalNumberOfElementsToDegenerate = 720 * 360;
        break;
    case ElementDegeneracyType::NEURONS_DEACTIVATE:
        if (fieldToDegenerate == "perceptual")
        {
            typeOfElementsDegenerated = "pre-synaptic neurons";
            //totalNumberOfElementsToDegenerate = 720;
        }
        else if (fieldToDegenerate == "decision")
        {
            typeOfElementsDegenerated = "post-synaptic neurons";
            //totalNumberOfElementsToDegenerate = 360;
        }
        degeneracyName = "deactivate";
        //numberOfElementsToDegeneratePerIteration = 1;
        break;
    default:
        break;
    }
    degeneracyName = degeneracyName + " " + typeOfElementsDegenerated;
}

void ExperimentParameters::print()
{
    std::ostringstream logStream;

    logStream << "Starting the experiment." << std::endl;
    logStream << "----------------------------------------" << std::endl;
    logStream << "Data saving is " << (isDataSavingOn ? "on" : "off") << std::endl;
    logStream << "Debug mode is " << (isDebugModeOn ? "on" : "off") << std::endl;
    logStream << "Visualization is " << (isVisualisationOn ? "on" : "off") << std::endl;
    logStream << "Number of trials: " << numberOfTrials << std::endl;
    logStream << "Decision tolerance: " << decisionTolerance << std::endl;
    logStream << "Degeneracy type: " << degeneracyName << std::endl;
    logStream << "Number of elements to degenerate per iteration: " <<numberOfElementsToDegeneratePerIteration << std::endl;
    logStream << "Total number of elements to degenerate: " << totalNumberOfElementsToDegenerate << std::endl;
    logStream << "Initial percentage of degeneration: " << initialPercentageOfDegeneration << std::endl;
    logStream << "Target percentage of degeneration: " << targetPercentageOfDegeneration << std::endl;
    logStream << "----------------------------------------" << std::endl;
    log(dnf_composer::tools::logger::LogLevel::INFO, logStream.str());

}