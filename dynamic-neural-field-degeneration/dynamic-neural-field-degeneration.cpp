#include "dynamic-neural-field-degeneration.h"

int main()
{
    ThreadHandler handler;

    // Start the threads
    handler.startThreads();

    // Wait for the threads to complete
    handler.joinThreads();

    return 0;
}
