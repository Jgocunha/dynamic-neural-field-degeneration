
#include "dynamic-neural-field-degeneration.h"

int main()
{
    try
    {
        //<----- parameters that change the experiment completely
        ExperimentParameters params;

        params.numberOfShapesPerTrial = 7;
        params.numberOfTrials = 10;
        params.decisionTolerance = 10;

        params.degeneracyType = ElementDegeneracyType::WEIGHTS_DEACTIVATE;
        params.fieldToDegenerate = "perceptual";

        params.initialPercentageOfDegeneration = 40;
        params.targetPercentageOfDegeneration = 100;
        params.incrementOfDegenerationPercentage = 10;

        params.relearningType = RelearningParameters::RelearningType::ONLY_DEGENERATED_CASES; //<------
        params.learningRate = 0.02;
        params.numberOfRelearningEpochs = 10; //<------
        params.maximumAmountOfDemonstrations = 10; //<------
        params.updateAllWeights = false; //<------

        params.isDataSavingOn = true;
        params.isComposerVisualizationOn = false;
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

