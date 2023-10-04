
#include "dynamic-neural-field-degeneration.h"

int main()
{
    try
    {
        //<----- parameters that change the experiment completely
        ExperimentParameters params;

        params.numberOfShapesPerTrial = 7;
        params.numberOfTrials = 100;
        params.decisionTolerance = 5;

        params.degeneracyType = ElementDegeneracyType::NEURONS_DEACTIVATE;
        params.fieldToDegenerate = "decision";

        params.initialPercentageOfDegeneration = 0;
        params.targetPercentageOfDegeneration = 100;
        params.incrementOfDegenerationPercentage = 1;

        params.relearningType = RelearningParameters::RelearningType::ONLY_DEGENERATED_CASES; //<------
        params.learningRate = 0.01;
        params.numberOfRelearningEpochs = 100; //<------
        params.maximumAmountOfDemonstrations = 10; //<------
        params.updateAllWeights = true; //<------

        params.isDataSavingOn = false;
        params.isComposerVisualizationOn = true;
        params.isDebugModeOn = true;
        params.isLinkToCoppeliaSimOn = false;

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

