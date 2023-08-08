
#include "dynamic-neural-field-degeneration.h"


int main()
{
    try
    {
        ExperimentParameters params;
        params.numberOfShapesPerTrial = 2;
        params.numberOfTrials = 1;
        params.percentageOfDegeneration = 20;
        params.degeneracyType = ElementDegeneracyType::NEURONS_DEACTIVATE;

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

