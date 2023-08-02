#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>

#include "./coppeliasimHandler.h"
#include "./dnfcomposerHandler.h"
#include "./dnfarchitecture.h"


#define TIMETOSETTLE_DEBUG 30
#define TIMETOSETTLE_FAST 30

#define TIMETOSLEEP_DEBUG 2200
#define TIMETOSLEEP_FAST 150


enum struct SimulationMode
{
    DEBUG = 0,
    FAST,
};

struct ExperimentParameters
{
    int numOfTrials = 1;
    SimulationMode mode = SimulationMode::DEBUG;
    ElementDegeneracyType degeneracyType = ElementDegeneracyType::NONE;
    //ElementLearningType tbdf
    double percentageOfElementsToAffect;
};

class ThreadHandler
{
private:
    std::thread dnfcomposerThread, coppeliasimThread;
    std::shared_ptr<DNFComposerHandler> dnfch;
protected:
    ExperimentParameters experimentParameters;
    int currentTrial = 1;
    std::mutex mtx;
    std::condition_variable cv;
    bool isReady = false;
    double cuboidHue = -1;
    double targetPlaceAngle = -1;
public:
    ThreadHandler(const ExperimentParameters& experimentParameters);
    ~ThreadHandler();

    void startThreads();
    void joinThreads();

private:
    void coppeliasimMain();
    void dnfcomposerMain();
    void dnfcomposerSignalHandling();
};
