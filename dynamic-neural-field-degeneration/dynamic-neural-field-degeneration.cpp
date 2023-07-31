#include "dynamic-neural-field-degeneration.h"

int main()
{
    int numTrials = 100;
    SimulationMode mode = SimulationMode::NORMAL;
    ThreadHandler handler{ numTrials, SimulationMode::NORMAL };

    Sleep(200);

    // Start the threads
    handler.startThreads();

    // Wait for the threads to complete
    handler.joinThreads();

    return 0;
}
