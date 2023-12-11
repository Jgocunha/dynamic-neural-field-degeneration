
#include "dynamic-neural-field-degeneration.h"

ExperimentParameters setExperimentParameters()
{
    ExperimentParameters params;

    std::cout << "Enter number of trials: ";
    std::cin >> params.numberOfTrials;

    std::cout << "Enter degeneracy type (1 for NEURONS_DEACTIVATE, "
        << "3 for WEIGHTS_DEACTIVATE, "
        << "5 for WEIGHTS_RANDOMIZE, "
        << "7 for WEIGHTS_REDUCE): ";
    int degeneracyTypeInput;
    std::cin >> degeneracyTypeInput;
    params.degeneracyType = static_cast<dnf_composer::element::ElementDegeneracyType>(degeneracyTypeInput);

    if (degeneracyTypeInput == 1)
    {
        std::cout << "Enter field to degenerate (perceptual/decision): ";
        std::cin >> params.fieldToDegenerate;
    }

	//params.numberOfTrials = 5;
    //params.degeneracyType = ElementDegeneracyType::NEURONS_DEACTIVATE;
    //params.fieldToDegenerate = "decision";
    setDegeneracyNameAndTypeOfElements(params);


	params.decisionTolerance = 2.0;
    params.startingExternalStimulus = 0;

	params.isDataSavingOn = false;
    params.isVisualisationOn = true;
    params.isDebugModeOn = true;

    return params;
}

void setDegeneracyNameAndTypeOfElements(ExperimentParameters& params)
{
    switch(params.degeneracyType)
    {
		case dnf_composer::element::ElementDegeneracyType::WEIGHTS_DEACTIVATE:
    		params.degeneracyName = "deactivate";
            params.typeOfElementsDegenerated = "weights";
    		break;
	    case dnf_composer::element::ElementDegeneracyType::WEIGHTS_RANDOMIZE:
    		params.degeneracyName = "randomize";
            params.typeOfElementsDegenerated = "weights";
    		break;
        case dnf_composer::element::ElementDegeneracyType::WEIGHTS_REDUCE:
            params.degeneracyName = "reduce 0.2";
            params.typeOfElementsDegenerated = "weights";
            break;
	    case dnf_composer::element::ElementDegeneracyType::NEURONS_DEACTIVATE:
            if(params.fieldToDegenerate == "perceptual")
				params.typeOfElementsDegenerated = "pre-synaptic neurons";
			else if(params.fieldToDegenerate == "decision")
				params.typeOfElementsDegenerated = "post-synaptic neurons";
            params.degeneracyName = "deactivate";
    		break;
	    default:
            break;
    }
	params.degeneracyName = params.degeneracyName + " " + params.typeOfElementsDegenerated;
}

int main()
{
    try
    {
        const ExperimentParameters params = setExperimentParameters();
        ExperimentHandler experiment { params };

        experiment.init();
        experiment.close();

        return 0;
    }
    catch (const dnf_composer::Exception& ex) {
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

