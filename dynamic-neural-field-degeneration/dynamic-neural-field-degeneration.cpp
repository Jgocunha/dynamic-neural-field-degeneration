#include "dynamic-neural-field-degeneration.h"



int main()
{
    ExperimentParameters expParam;

    expParam.numOfTrials = 100;
    expParam.mode = SimulationMode::DEBUG;
    expParam.degeneracyType = ElementDegeneracyType::WEIGHTS_DEACTIVATE;

    try
    {
        ThreadHandler handler{expParam};

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

