#include "../include/experimentHandler.h"


ExperimentHandler::ExperimentHandler(const ExperimentParameters& expParam)
    : expParam{ expParam }
{
    DNFarchitecture dnfarch;

    // Depending on SimulationMode show the interface or not
    if (expParam.mode == SimulationMode::DEBUG)
        dnfch = std::make_shared<DNFComposerHandler>(dnfarch.getSimulation(), true, expParam.timeForFieldsToSettle);
    else
        dnfch = std::make_shared<DNFComposerHandler>(dnfarch.getSimulation(), false, expParam.timeForFieldsToSettle);
}

ExperimentHandler::~ExperimentHandler()
{
}

void ExperimentHandler::startThreads()
{
    // Create and start the threads
    coppeliasimThread = std::thread(&ExperimentHandler::coppeliasimMain, this);
    dnfcomposerThread = std::thread(&ExperimentHandler::dnfcomposerMain, this);
}

void ExperimentHandler::joinThreads()
{
    // Wait for the threads to finish their execution
    coppeliasimThread.join();
    dnfcomposerThread.join();

    // Output after both threads have completed
    std::cout << "Threads have completed their execution\n";
}

void ExperimentHandler::coppeliasimMain()
{
    CoppeliasimHandler cpsh{ expParam.numOfTrials };

    while (!cpsh.initialize());

    while (expData.currentTrial <= expParam.numOfTrials)
    {
        cpsh.createShape();

        cpsh.setShapeHandle("Cuboid_" + std::to_string(expData.currentTrial));
        cpsh.setShapeHue();

        {
            // Lock the mutex before accessing the shared variables
            std::unique_lock<std::mutex> lock(signals.mtx);

            // Write to cuboidHue
            expData.cuboidHue = cpsh.getShapeHue();

            // Notify dnfcomposer thread that cuboidHue is ready
            signals.cuboidHueIsRead = true;
            signals.cv.notify_one();

            // Wait for dnfcomposer to finish reading cuboidHue and write to targetPlaceAngle
            signals.cv.wait(lock, [this]() { return !signals.cuboidHueIsRead; });
        }

        cpsh.pickUpShape();

        // Use the value read from cuboidHue
        cpsh.setTargetAngle(expData.targetPlaceAngle);

        cpsh.placeShape();

        cpsh.resetSignals();

        expData.currentTrial++;
    }
    cpsh.stop();
}

void ExperimentHandler::dnfcomposerMain()
{
    dnfch->init();

    std::thread dnfcomposerSignalHandlingThread(&ExperimentHandler::dnfcomposerSignalHandling, this);

    // Before starting the trials degenerate target percentage
    for (int i = 0; i < expParam.percentageOfElementsToAffect; i++)
        dnfch->applyDegenerationAtTheBeginning(expParam.degeneracyType);

    while (expData.currentTrial <= expParam.numOfTrials)
    {
        dnfch->step();

        if (expData.cuboidHue >= 0)
        {
            dnfch->setExternalStimulus(expData.cuboidHue);
            expData.cuboidHue = -1;

            if (expParam.degeneracyType != ElementDegeneracyType::NONE)
                dnfch->applyDegeneration(expParam.degeneracyType);
        }

    }

    dnfch->saveCentroids();

    // Wait for dnfcomposerSignalHandlingThread to complete
    dnfcomposerSignalHandlingThread.join(); 

    dnfch->close();

}

void ExperimentHandler::dnfcomposerSignalHandling()
{
    std::unique_lock<std::mutex> lock(signals.mtx);

    while (expData.currentTrial <= expParam.numOfTrials)
    {
        // Wait until timeForSimToSleep is true or until the condition_variable is notified,
        // whichever comes first.
        signals.cv.wait(lock, [this]() { return signals.cuboidHueIsRead || dnfch->getUserRequestClose(); });

        // Check if the loop is still running
        if (!dnfch->getUserRequestClose())
        {
            // Simulate some processing time
            if(expParam.mode == SimulationMode::DEBUG)
                std::this_thread::sleep_for(std::chrono::milliseconds(expParam.timeForSimToSleep));
            else
                std::this_thread::sleep_for(std::chrono::milliseconds((expParam.timeForSimToSleep - 200)/100 - 50));

            // Write the targetPlaceAngle from the DNF composer to the shared variable
            expData.targetPlaceAngle = dnfch->getTargetPlaceAngle();

            // Notify thread 1 that var2 is ready
            signals.cuboidHueIsRead = false;
            signals.cv.notify_one();
        }
    }
}