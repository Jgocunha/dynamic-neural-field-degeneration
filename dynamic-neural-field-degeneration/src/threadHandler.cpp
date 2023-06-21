#include "../include/threadHandler.h"


ThreadHandler::ThreadHandler(const int& numTrials)
    : numTrials{ numTrials }
{
    DNFarchitecture dnfarch;
    dnfarch.setup();

    std::shared_ptr<Simulation> simulation = dnfarch.getSimulation();

    dnfch = DNFComposerHandler{ simulation };
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

    if (cpsh.initialize())
    {
        while (currentTrial <= numTrials)
        {
            cpsh.createShape();

            cpsh.setShapeHandle("Cuboid_" + std::to_string(currentTrial));
            cpsh.getShapeParameters();

            // Lock the mutex before accessing the shared variables
            std::unique_lock<std::mutex> lock(mtx);

            // Write to cuboidColor
            cuboidColor = cpsh.getShapeColor();

            // Notify dnfcomposer thread that cuboidColor is ready
            isReady = true;
            cv.notify_one();

            // Wait for dnfcomposer to finish reading cuboidColor and write to targetBox
            cv.wait(lock, [this]() { return !isReady; });

            cpsh.pickUpShape();

            // Use the value read from targetBox
            cpsh.setTargetBox(targetBox);

            cpsh.placeShape();

            cpsh.resetSignals();
            //cpsh.endStep();
            currentTrial++;
        }
        cpsh.stop();
    }
}

int ThreadHandler::dnfcomposerMain()
{
    try {

        dnfch.init();
        
        std::thread dnfcomposerSignalHandlingThread = std::thread(&ThreadHandler::dnfcomposerSignalHandling, this);

        while (!dnfch.getUserRequestClose())
        {
            dnfch.step();

            if (cuboidColor != "UNDEFINED")
                dnfch.setExternalStimulus(cuboidColor);
            cuboidColor = "UNDEFINED";
      
        }
        dnfch.close();

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


void ThreadHandler::dnfcomposerSignalHandling()
{
    //Lock the mutex before accessing the shared variables
    std::unique_lock<std::mutex> lock(mtx);

    while (!dnfch.getUserRequestClose())
    {
        // Wait for var1 to be ready for a specified duration
        if (cv.wait_for(lock, std::chrono::milliseconds(10), [this]() { return isReady; }))
        {
            //// Read var1
            //std::string color = cuboidColor;

            //std::cout << "Cuboid color was read in thread2 as: " + cuboidColor << "\n";

            //dnfch.setExternalStimulus(cuboidColor);

            Sleep(1000);

            // Write to var2
            targetBox = dnfch.getTargetBox();

            // Notify thread 1 that var2 is ready
            isReady = false;
            cv.notify_one();
        }
    }
}