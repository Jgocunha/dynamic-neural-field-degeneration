
#include "dynamic-neural-field-degeneration.h"

int main()
{
    try
    {
        ExperimentParameters params;
        params.numberOfShapesPerTrial = 7;
        params.numberOfTrials = 5;
        params.decisionTolerance = 2;
        params.degeneracyType = ElementDegeneracyType::WEIGHTS_RANDOMIZE;
        params.degeneracyName = "weights-randomize";

        params.initialPercentageOfDegeneration = 0;
        params.percentageOfDegeneration = 10;
        params.numberOfTenthsOfPercentageToDegenerate = 20;

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

