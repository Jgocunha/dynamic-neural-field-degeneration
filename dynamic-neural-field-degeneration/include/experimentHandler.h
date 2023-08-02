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
    DEBUG = 0,
    FAST,
};

struct ExperimentParameters
{
    int numOfTrials = 1;
    SimulationMode mode = SimulationMode::DEBUG;
    ElementDegeneracyType degeneracyType = ElementDegeneracyType::NONE;
    double percentageOfElementsToAffect = 0.0;
    double acceptableAngleError = 5.0;
    
    int timeForFieldsToSettle = 30;
    int timeForSimToSleep = 2200;
};

struct ExperimentData
{
    int currentTrial = 1;
    double cuboidHue = -1.0;
    double targetPlaceAngle = -1.0;
    const int numberOfDifferentCuboids = 7;
    std::vector<double> registeredAngles;
    std::vector<double> expectedAngles = { 15, 40, 65, 90, 115, 140, 165 };
};

struct ThreadSignalling
{
    bool cuboidHueIsRead = false;
    bool dnfcomposerIsReady = false;
    bool coppeliaIsReady = false;
    std::mutex mtx;
    std::condition_variable cv;
};

class ExperimentHandler
{
private:
    std::thread dnfcomposerThread, coppeliasimThread;
    std::shared_ptr<DNFComposerHandler> dnfch;
protected:
    ExperimentParameters expParam;
    ExperimentData expData;
    ThreadSignalling signals;
public:
    ExperimentHandler(const ExperimentParameters& expParam);
    ~ExperimentHandler();

    void startThreads();
    void joinThreads();
private:
    void coppeliasimMain();
    void dnfcomposerMain();
    void dnfcomposerSignalHandling();

    void taskProcedure();
    bool evaluateBehaviour();
    void degenerationProcedure();
    void learningProcedure();
};
