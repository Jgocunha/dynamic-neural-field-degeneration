#include "dynamic-neural-field-composer.h"

// This .cpp file is an example of how you can use the library to create your own DNF simulation.

int main(int argc, char* argv[])
{
    // After defining the simulation, we can create the application.
    std::shared_ptr<Simulation> simulation = test_DegeneracyCuboidColor();
    // You can run the application without the user interface by setting the second parameter to false.
    Application app{ simulation, true};
    
    // After creating the application, we can add the windows we want to display.
    app.activateUserInterfaceWindow(std::make_shared<SimulationWindow>(simulation));
    PlotDimensions pd;
    pd = { 0, 360, -30, 40 };
    app.activateUserInterfaceWindow(std::make_shared<PlotWindow>(simulation, pd));
    pd = { 0, 180, -30, 40 };
    app.activateUserInterfaceWindow(std::make_shared<PlotWindow>(simulation, pd));
    app.activateUserInterfaceWindow(std::make_shared<DegeneracyWindow>(simulation));

    try {
        app.init();

        bool userRequestClose = false;
        while (!userRequestClose)
        {
            app.step();
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