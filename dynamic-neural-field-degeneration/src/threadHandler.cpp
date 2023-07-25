#include "../include/threadHandler.h"


ThreadHandler::ThreadHandler(int numTrials)
    : numTrials{ numTrials }
{
    DNFarchitecture dnfarch;
    dnfch = std::make_shared<DNFComposerHandler>(dnfarch.getSimulation());
}

ThreadHandler::~ThreadHandler()
{
}

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

    while (!cpsh.initialize());

    while (currentTrial <= numTrials)
    {
        cpsh.createShape();

        cpsh.setShapeHandle("Cuboid_" + std::to_string(currentTrial));
        cpsh.setShapeHue();

        {
            // Lock the mutex before accessing the shared variables
            std::unique_lock<std::mutex> lock(mtx);

            // Write to cuboidHue
            cuboidHue = cpsh.getShapeHue();

            // Notify dnfcomposer thread that cuboidHue is ready
            isReady = true;
            cv.notify_one();

            // Wait for dnfcomposer to finish reading cuboidHue and write to targetPlaceAngle
            cv.wait(lock, [this]() { return !isReady; });
        }

        cpsh.pickUpShape();

        // Use the value read from cuboidHue
        cpsh.setTargetAngle(targetPlaceAngle);

        cpsh.placeShape();

        cpsh.resetSignals();

        currentTrial++;
    }
    cpsh.stop();
}

int ThreadHandler::dnfcomposerMain()
{
    try {
        dnfch->init();

        std::thread dnfcomposerSignalHandlingThread(&ThreadHandler::dnfcomposerSignalHandling, this);

        while (!dnfch->getUserRequestClose())
        {
            dnfch->step();

            if (cuboidHue >= 0)
            {
                dnfch->setExternalStimulus(cuboidHue);
                cuboidHue = -1;
            }
        }

        // Wait for dnfcomposerSignalHandlingThread to complete
        dnfcomposerSignalHandlingThread.join(); 

        dnfch->close();

        return 0;
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

void ThreadHandler::dnfcomposerSignalHandling()
{
    std::unique_lock<std::mutex> lock(mtx);

    while (!dnfch->getUserRequestClose())
    {
        // Wait until isReady is true or until the condition_variable is notified,
        // whichever comes first.
        cv.wait(lock, [this]() { return isReady || dnfch->getUserRequestClose(); });

        // Check if the loop is still running
        if (!dnfch->getUserRequestClose())
        {
            // Simulate some processing time
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            // Write the targetPlaceAngle from the DNF composer to the shared variable
            targetPlaceAngle = dnfch->getTargetPlaceAngle();

            // Notify thread 1 that var2 is ready
            isReady = false;
            cv.notify_one();
        }
    }
}