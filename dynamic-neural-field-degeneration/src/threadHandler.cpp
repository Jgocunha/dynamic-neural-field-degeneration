#include "../include/threadHandler.h"


ThreadHandler::ThreadHandler(const ExperimentParameters& experimentParameters)
    : experimentParameters{ experimentParameters }
{
    DNFarchitecture dnfarch;
    if (experimentParameters.mode == SimulationMode::DEBUG)
        dnfch = std::make_shared<DNFComposerHandler>(dnfarch.getSimulation(), true, TIMETOSETTLE_DEBUG);
    else
        dnfch = std::make_shared<DNFComposerHandler>(dnfarch.getSimulation(), false, TIMETOSETTLE_FAST);
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
    CoppeliasimHandler cpsh{ experimentParameters.numOfTrials };

    while (!cpsh.initialize());

    while (currentTrial <= experimentParameters.numOfTrials)
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

void ThreadHandler::dnfcomposerMain()
{
    dnfch->init();

    std::thread dnfcomposerSignalHandlingThread(&ThreadHandler::dnfcomposerSignalHandling, this);

    // Before starting the trials degenerate target percentage
    for (int i = 0; i < experimentParameters.percentageOfElementsToAffect; i++)
        dnfch->applyDegenerationAtTheBeginning(experimentParameters.degeneracyType);

    while (currentTrial <= experimentParameters.numOfTrials)
    {
        dnfch->step();

        if (cuboidHue >= 0)
        {
            dnfch->setExternalStimulus(cuboidHue);
            cuboidHue = -1;

            if (experimentParameters.degeneracyType != ElementDegeneracyType::NONE)
                dnfch->applyDegeneration(experimentParameters.degeneracyType);

            //if(experimentParameters.learningType != ElementDegeneracyType::NONE) tbdf
        }

    }

    dnfch->saveCentroids();

    // Wait for dnfcomposerSignalHandlingThread to complete
    dnfcomposerSignalHandlingThread.join(); 

    dnfch->close();

}

void ThreadHandler::dnfcomposerSignalHandling()
{
    std::unique_lock<std::mutex> lock(mtx);

    while (currentTrial <= experimentParameters.numOfTrials)
    {
        // Wait until isReady is true or until the condition_variable is notified,
        // whichever comes first.
        cv.wait(lock, [this]() { return isReady || dnfch->getUserRequestClose(); });

        // Check if the loop is still running
        if (!dnfch->getUserRequestClose())
        {
            // Simulate some processing time
            if(experimentParameters.mode == SimulationMode::DEBUG)
                std::this_thread::sleep_for(std::chrono::milliseconds(TIMETOSLEEP_DEBUG));
            else
                std::this_thread::sleep_for(std::chrono::milliseconds(TIMETOSLEEP_FAST));

            // Write the targetPlaceAngle from the DNF composer to the shared variable
            targetPlaceAngle = dnfch->getTargetPlaceAngle();

            // Notify thread 1 that var2 is ready
            isReady = false;
            cv.notify_one();
        }
    }
}