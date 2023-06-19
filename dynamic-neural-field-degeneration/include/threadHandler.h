#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>

#include "./coppeliasimHandler.h"

class ThreadHandler
{
private:
    std::thread dnfcomposerThread, coppeliasimThread;
protected:
    std::mutex mtx;
    std::condition_variable cv;
    bool isReady = false;
    std::string cuboidColor = "UNDEFINED";
    std::string targetBox = "UNDEFINED";
public:
    ThreadHandler() {}
    ~ThreadHandler() {}

    void startThreads();
    void joinThreads();

private:
    void threadFunction1() 
    {
        CoppeliasimHandler cpsh{10};
            
        if (cpsh.initialize())
        {
            cpsh.run();
			cpsh.stop();
        }

        //// Lock the mutex before accessing the shared variables
        //std::unique_lock<std::mutex> lock(mtx);

        //// Write to var1
        //cuboidColor = "RED";

        //// Notify thread 2 that var1 is ready
        //isReady = true;
        //cv.notify_one();

        //// Wait for thread 2 to finish reading var1 and write to var2
        //cv.wait(lock, [this]() { return !isReady; });

        //// Use the value read from var2
        //std::cout << "Thread 1: var2 = " << targetBox << "\n";
    }

    // Function to be executed in the second thread
    void threadFunction2() {
        //// Perform some computations or tasks

        //// Lock the mutex before accessing the shared variables
        //std::unique_lock<std::mutex> lock(mtx);

        //// Wait for var1 to be ready
        //cv.wait(lock, [this]() { return isReady; });

        //// Read var1
        //std::string color = cuboidColor;

        //std::cout << "Cuboid color was read in thread2 as: " + color << "\n";

        //// Write to var2
        //targetBox = "BOX_1";

        //// Notify thread 1 that var2 is ready
        //isReady = false;
        //cv.notify_one();
    }
};