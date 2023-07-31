#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>

#include "./coppeliasimHandler.h"
#include "./dnfcomposerHandler.h"
#include "./dnfarchitecture.h"

enum struct SimulationMode
{
    NORMAL = 0,
    DEGENERATE,
    LEARNING
};


class ThreadHandler
{
private:
    std::thread dnfcomposerThread, coppeliasimThread;
    std::shared_ptr<DNFComposerHandler> dnfch;
protected:
    int numTrials;
    SimulationMode simMode;
    bool learning;
    int currentTrial = 1;
    std::mutex mtx;
    std::condition_variable cv;
    bool isReady = false;
    double cuboidHue = -1;
    double targetPlaceAngle = -1;
public:
    ThreadHandler(int numTrials, SimulationMode simMode);
    ~ThreadHandler();

    void startThreads();
    void joinThreads();

private:
    void coppeliasimMain();
    int dnfcomposerMain();
    void dnfcomposerSignalHandling();
};
