
#include "dynamic-neural-field-degeneration.h"

int main()
{
    try
    {
        ExperimentParameters params;
        params.numberOfTrials = 2;
        params.decisionTolerance = 2;

        params.degeneracyType = ElementDegeneracyType::NEURONS_DEACTIVATE;
        params.degeneracyName = "NEURONS_DEACTIVATE";
        params.typeOfElementsDegenerated = "neurons";

        params.targetExternalStimulusPosition = 0;
        params.targetOutputCentroid = 15;

        params.initialPercentageOfDegeneration = 0;
        params.targetPercentageOfDegeneration = 100;

        params.isDataSavingOn = true;
        params.isVisualisationOn = true;
        params.isDebugModeOn = false;

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

