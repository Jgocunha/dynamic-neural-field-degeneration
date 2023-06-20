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
    int numTrials;
    int currentTrial = 1;
public:
    ThreadHandler(const int& numTrials = 1) :numTrials(numTrials) {}
    ~ThreadHandler() {}

    void startThreads();
    void joinThreads();

private:
    void coppeliasimMain();

    // Function to be executed in the second thread
    int dnfcomposerMain() 
    {
        std::shared_ptr<Simulation> simulation = std::make_shared<Simulation>();

        std::vector<std::shared_ptr<Visualization>> visualizations;
        visualizations.push_back(std::make_shared<Visualization>(simulation));

        Application app{ simulation, visualizations, true };

        try {
            app.init();

            bool userRequestClose = false;
            while (!userRequestClose)
            {
                app.step();

                //Lock the mutex before accessing the shared variables
                std::unique_lock<std::mutex> lock(mtx);

                // Wait for var1 to be ready for a specified duration
                if (cv.wait_for(lock, std::chrono::milliseconds(100), [this]() { return isReady; })) {
                    // Read var1
                    std::string color = cuboidColor;

                    std::cout << "Cuboid color was read in thread2 as: " + color << "\n";

                    // Write to var2
                    targetBox = "BOX_1";

                    // Notify thread 1 that var2 is ready
                    isReady = false;
                    cv.notify_one();
                }

                userRequestClose = app.getCloseUI();
            }
            app.close();
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
};

//while (currentTrial <= numTrials)
        //{ 
        //    // Lock the mutex before accessing the shared variables
        //    std::unique_lock<std::mutex> lock(mtx);

        //    // Wait for var1 to be ready
        //    cv.wait(lock, [this]() { return isReady; });

        //    // Read var1
        //    std::string color = cuboidColor;

        //    std::cout << "Cuboid color was read in thread2 as: " + color << "\n";

        //    // Write to var2
        //    targetBox = "BOX_1";

        //    // Notify thread 1 that var2 is ready
        //    isReady = false;
        //    cv.notify_one();
        //}