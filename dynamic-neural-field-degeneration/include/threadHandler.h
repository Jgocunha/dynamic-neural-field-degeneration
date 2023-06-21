#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>

#include "./coppeliasimHandler.h"
#include "./dnfcomposerHandler.h"
#include "./dnfarchitecture.h"

class ThreadHandler
{
private:
    std::thread dnfcomposerThread, coppeliasimThread;
    DNFComposerHandler dnfch;
protected:
    std::mutex mtx;
    std::condition_variable cv;
    bool isReady = false;
    std::string cuboidColor = "UNDEFINED";
    std::string targetBox = "UNDEFINED";
    int numTrials;
    int currentTrial = 1;
public:
    ThreadHandler(const int& numTrials = 1);
    ~ThreadHandler() {}

    void startThreads();
    void joinThreads();

private:
    void coppeliasimMain();
    int dnfcomposerMain();
    void dnfcomposerSignalHandling();
};
