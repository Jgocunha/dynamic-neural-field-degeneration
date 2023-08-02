
#include "dynamic-neural-field-degeneration.h"

int main()
{
    ExperimentParameters expParam;

    expParam.numOfTrials = 100;
    expParam.mode = SimulationMode::DEBUG;
    expParam.degeneracyType = ElementDegeneracyType::NEURONS_DEACTIVATE_PERCENTAGE;
    expParam.percentageOfElementsToAffect = 15;
    expParam.acceptableAngleError = 5;
    
    expParam.timeForFieldsToSettle = 30;
    expParam.timeForSimToSleep = 2200;

    try
    {
        ExperimentHandler handler{expParam};

        // Wait just a little bit for the architecture to setup
        Sleep(200);

        // Start the threads
        handler.startThreads();

        // Wait for the threads to complete
        handler.joinThreads();

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

    return 0;
}

