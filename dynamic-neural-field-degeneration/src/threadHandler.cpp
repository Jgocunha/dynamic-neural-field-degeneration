#include "../include/threadHandler.h"

void ThreadHandler::startThreads()
{
    // Create and start the threads
    dnfcomposerThread = std::thread(&ThreadHandler::threadFunction1, this);
    coppeliasimThread = std::thread(&ThreadHandler::threadFunction2, this);
}

void ThreadHandler::joinThreads()
{
    // Wait for the threads to finish their execution
    dnfcomposerThread.join();
    coppeliasimThread.join();

    // Output after both threads have completed
    std::cout << "Threads have completed their execution\n";
}