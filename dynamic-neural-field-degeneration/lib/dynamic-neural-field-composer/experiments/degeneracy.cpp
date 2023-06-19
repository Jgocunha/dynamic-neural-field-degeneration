#include "degeneracy.h"

int main(int argc, char* argv[])
{
    DegeneracyExperimentUserParameters deup;
    deup.simulation = test_DegeneracyOneLayerFieldModel();
    deup.degeneracyType = ElementDegeneracyType::NEURONS_DEACTIVATE;
    deup.numTrials = 500;
    deup.stimulusLocations = { 31.5 };
    deup.timeForFieldsToSettle = 50;
    deup.debug = false;
    deup.visualize = true;
    deup.saveData = false;
    deup.weightReductionFactor = 0.4;
    deup.idOfFieldToDegenerate = "field v";
    deup.idOfFieldToObserve = "field v";

    DegeneracyExperiment exp{ deup };

	try 
    {
        exp.run();
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

    return 0;
}