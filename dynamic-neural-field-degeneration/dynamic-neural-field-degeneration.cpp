// dynamic-neural-field-degeneration.cpp : Defines the entry point for the application.
//

#include "dynamic-neural-field-degeneration.h"

#include "./lib/dynamic-neural-field-composer/include/application/application.h"
#include "./lib/dynamic-neural-field-composer/examples/examples.h"

using namespace std;

int main()
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
