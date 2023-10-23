
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
        params.degeneracyType = static_cast<ElementDegeneracyType>(degeneracyTypeInput);

        if (degeneracyTypeInput == 1)
        {
            std::cout << "Enter field to degenerate (perceptual/decision): ";
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
        params.numberOfTrials = 1;
        params.degeneracyType = ElementDegeneracyType::NEURONS_DEACTIVATE;
        params.fieldToDegenerate = "perceptual";
        params.initialPercentageOfDegeneration = 0;
        params.incrementOfDegenerationPercentage = 1;
        params.numberOfRelearningEpochs = 1;
        params.maximumAmountOfDemonstrations = 1;
    }

    return params;
}


int main()
{
    try
    {
        ExperimentParameters params = inputUserParameters();

        params.numberOfShapesPerTrial = 7;
        params.decisionTolerance = 10;

        params.targetPercentageOfDegeneration = 100;
        params.learningRate = 0.02;
        params.relearningType = RelearningParameters::RelearningType::ONLY_DEGENERATED_CASES;
        params.updateAllWeights = false; 

        params.isDataSavingOn = false;
        params.isComposerVisualizationOn = true;
        params.isDebugModeOn = true;
        params.isLinkToCoppeliaSimOn = true;

        ExperimentHandler experiment{ params };
        
        experiment.init();
        experiment.close();

        return 0;
    }
    catch (const Exception& ex) {
        std::cerr << "Exception: " << ex.what() << " ErrorCode: " << static_cast<int>(ex.getErrorCode()) << std::endl;
        return static_cast<int>(ex.getErrorCode());
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception occurred." << std::endl;
        return 1;
    }
}

