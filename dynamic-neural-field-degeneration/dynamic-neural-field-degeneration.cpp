
#include "dynamic-neural-field-degeneration.h"

int main()
{
    try
    {
        ExperimentParameters params;

        params.numberOfShapesPerTrial = 1;
        params.numberOfTrials = 1;
        params.decisionTolerance = 5;

        params.degeneracyType = ElementDegeneracyType::WEIGHTS_DEACTIVATE;
        params.fieldToDegenerate = "perceptual";

        params.initialPercentageOfDegeneration = 10;
        params.targetPercentageOfDegeneration = 100;
        params.incrementOfDegenerationPercentage = 10;

        params.maximumAmountOfRelearningCycles = 10;

        params.isDataSavingOn = true;
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

