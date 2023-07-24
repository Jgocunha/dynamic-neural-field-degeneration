#include "dynamic-neural-field-degeneration.h"

int main()
{
    int numTrials = 100;
    ThreadHandler handler{ numTrials };
    Sleep(200);

    // Start the threads
    handler.startThreads();

    // Wait for the threads to complete
    handler.joinThreads();

    return 0;
}
