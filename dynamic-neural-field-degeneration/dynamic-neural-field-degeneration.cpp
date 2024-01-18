
#include "dynamic-neural-field-degeneration.h"


ExperimentParameters inputUserParameters()
{
    ExperimentParameters params;

    std::cout << "Select operation mode (0 for automatic, "
        << "1 for manual): ";
    int operationMode;
    std::cin >> operationMode;

    if(operationMode)
    {
        std::cout << "Enter number of trials: ";
        std::cin >> params.numberOfTrials;

        std::cout << "Enter degeneracy type (1 for NEURONS_DEACTIVATE, "
            << "3 for WEIGHTS_DEACTIVATE): ";
        int degeneracyTypeInput;
        std::cin >> degeneracyTypeInput;
        params.degeneracyType = static_cast<dnf_composer::element::ElementDegeneracyType>(degeneracyTypeInput);

        if (degeneracyTypeInput == 1)
        {
            std::cout << "Enter field to degenerate (perceptual/output): ";
            std::cin >> params.fieldToDegenerate;
        }
        else
            params.fieldToDegenerate = "null";

        std::cout << "Enter initial percentage of degeneration: ";
        std::cin >> params.initialPercentageOfDegeneration;

        std::cout << "Enter increment of degeneration percentage: ";
        std::cin >> params.incrementOfDegenerationPercentage;

        //std::cout << "Enter relearning type (0 for ONLY_DEGENERATED_CASES, 1 for ALL_CASES): ";
        //int relearningTypeInput;
        //std::cin >> relearningTypeInput;
        //params.relearningType = (relearningTypeInput == 0) ? RelearningParameters::RelearningType::ONLY_DEGENERATED_CASES : RelearningParameters::RelearningType::ALL_CASES;

        //std::cout << "Enter learning rate: ";
        //std::cin >> params.learningRate;

        std::cout << "Enter number of relearning epochs: ";
        std::cin >> params.numberOfRelearningEpochs;

        std::cout << "Enter maximum amount of demonstrations: ";
        std::cin >> params.maximumAmountOfDemonstrations;

        //std::cout << "Enter update all weights (0 for false, 1 for true): ";
        //int updateAllWeightsInput;
        //std::cin >> updateAllWeightsInput;
        //params.updateAllWeights = (updateAllWeightsInput == 1);
    }
    else
    {
        params.numberOfTrials = 50;
        params.degeneracyType = dnf_composer::element::ElementDegeneracyType::NEURONS_DEACTIVATE;
        params.fieldToDegenerate = "perceptual";
        params.initialPercentageOfDegeneration = 15;
        params.incrementOfDegenerationPercentage = 0.2;
        params.numberOfRelearningEpochs = 100;
        params.maximumAmountOfDemonstrations = 1;
    }

    return params;
}


int main()
{
    try
    {
        ExperimentParameters params = inputUserParameters();

        params.numberOfShapesPerTrial = 1;
        params.decisionTolerance = 2.00;

        params.targetPercentageOfDegeneration = 100;
        params.learningRate = 0.02;
        params.relearningType = RelearningParameters::RelearningType::ONLY_DEGENERATED_CASES;
        params.updateAllWeights = false; 

        params.isComposerVisualizationOn = false;
        params.isDataSavingOn = params.isComposerVisualizationOn;
        params.isDebugModeOn = true;
        params.isLinkToCoppeliaSimOn = false;

        ExperimentHandler experiment{ params };
        
        experiment.init();
        experiment.close();

        return 0;
    }
    catch (const dnf_composer::Exception& ex)
    {
        const std::string errorMessage = "Exception: " + std::string(ex.what()) + " ErrorCode: " + std::to_string(static_cast<int>(ex.getErrorCode())) + ". \n";
        log(dnf_composer::LogLevel::FATAL, errorMessage, dnf_composer::LogOutputMode::CONSOLE);
        return static_cast<int>(ex.getErrorCode());
    }
    catch (const std::exception& ex)
    {
        log(dnf_composer::LogLevel::FATAL, "Exception caught: " + std::string(ex.what()) + ". \n", dnf_composer::LogOutputMode::CONSOLE);
        return 1;
    }
    catch (...)
    {
        log(dnf_composer::LogLevel::FATAL, "Unknown exception occurred. \n", dnf_composer::LogOutputMode::CONSOLE);
        return 1;
    }
}

