#include "dynamic-neural-field-composer.h"

int main(int argc, char* argv[])
{
    //std::shared_ptr<Simulation> simulation = std::make_shared<Simulation>();
    //std::shared_ptr<Simulation> simulation = test_SharedTaskKnowledge();
    std::shared_ptr<Simulation> simulation = test_DegeneracyOneLayerFieldModel();

    std::vector<std::shared_ptr<Visualization>> visualizations;
    visualizations.push_back(std::make_shared<Visualization>(simulation));
    
    Application app{ simulation, visualizations, true};
    
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