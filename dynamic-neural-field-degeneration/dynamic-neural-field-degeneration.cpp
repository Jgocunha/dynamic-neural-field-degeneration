
#include "dynamic-neural-field-degeneration.h"


int main()
{
    try
    {
        ExperimentParameters params;

        params.numberOfStimulusPerTrial = 7;
        params.numberOfTrials = 1000;
        params.decisionTolerance = 5;

        params.degeneracyType = ElementDegeneracyType::NEURONS_DEACTIVATE;
        params.fieldToDegenerate = "perceptual";

        params.isDataSavingOn = false;
        params.isVisualizationOn = true;
        params.isDebugModeOn = true;

        ExperimentHandler experiment { params };

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

