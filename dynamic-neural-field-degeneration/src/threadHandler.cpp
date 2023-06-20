#include "../include/threadHandler.h"

void ThreadHandler::startThreads()
{
    // Create and start the threads
    coppeliasimThread = std::thread(&ThreadHandler::coppeliasimMain, this);
    dnfcomposerThread = std::thread(&ThreadHandler::dnfcomposerMain, this);
}

void ThreadHandler::joinThreads()
{
    // Wait for the threads to finish their execution
    coppeliasimThread.join();
    dnfcomposerThread.join();

    // Output after both threads have completed
    std::cout << "Threads have completed their execution\n";
}

void ThreadHandler::coppeliasimMain()
{
    CoppeliasimHandler cpsh{ numTrials };

    if (cpsh.initialize())
    {
        while (currentTrial <= numTrials)
        {
            cpsh.startStep(currentTrial);

            // Lock the mutex before accessing the shared variables
            std::unique_lock<std::mutex> lock(mtx);

            // Write to cuboidColor
            cuboidColor = cpsh.getShapeColor();

            // Notify dnfcomposer thread that cuboidColor is ready
            isReady = true;
            cv.notify_one();

            // Wait for dnfcomposer to finish reading cuboidColor and write to targetBox
            cv.wait(lock, [this]() { return !isReady; });

            // Use the value read from targetBox
            cpsh.setTargetBox(targetBox);

            cpsh.endStep();
            currentTrial++;
        }
        cpsh.stop();
    }
}
