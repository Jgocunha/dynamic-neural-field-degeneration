
#include "dynamic-neural-field-degeneration.h"

constexpr bool manualMode = true;

ExperimentParameters setExperimentParameters()
{
    ExperimentParameters params;

    if(manualMode)
    {
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
    }
    else
    {
    	params.numberOfTrials = 10;
		params.degeneracyType = dnf_composer::element::ElementDegeneracyType::NEURONS_DEACTIVATE;
		params.fieldToDegenerate = "perceptual";
    }

    setDegeneracyNameAndTypeOfElements(params);

	params.decisionTolerance = 2.0;
    params.startingExternalStimulus = 0;

    params.isVisualisationOn = true;
	params.isDataSavingOn = !params.isVisualisationOn;
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
			params.numberOfElementsToDegenerate = 1000;
    		break;
	    case dnf_composer::element::ElementDegeneracyType::WEIGHTS_RANDOMIZE:
    		params.degeneracyName = "randomize";
            params.typeOfElementsDegenerated = "weights";
			params.numberOfElementsToDegenerate = 1000;
    		break;
        case dnf_composer::element::ElementDegeneracyType::WEIGHTS_REDUCE:
            params.degeneracyName = "reduce 0.05";
            params.typeOfElementsDegenerated = "weights";
			params.numberOfElementsToDegenerate = 1000;
            break;
	    case dnf_composer::element::ElementDegeneracyType::NEURONS_DEACTIVATE:
            if(params.fieldToDegenerate == "perceptual")
				params.typeOfElementsDegenerated = "pre-synaptic neurons";
			else if(params.fieldToDegenerate == "decision")
				params.typeOfElementsDegenerated = "post-synaptic neurons";
            params.degeneracyName = "deactivate";
			params.numberOfElementsToDegenerate = 1;
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
	catch (const dnf_composer::Exception& ex)
	{
		const std::string errorMessage = "Exception: " + std::string(ex.what()) + " ErrorCode: " + std::to_string(static_cast<int>(ex.getErrorCode())) + ". \n";
		log(dnf_composer::LogLevel::FATAL, errorMessage, dnf_composer::LogOutputMode::CONSOLE);
		return static_cast<int>(ex.getErrorCode());
	}
	catch (const std::exception& ex)
	{
		log(dnf_composer::LogLevel::FATAL, "Exception caught: " + std::string(ex.what()) + ". \n", dnf_composer::LogOutputMode::CONSOLE);
		return 1;
	}
	catch (...)
	{
		log(dnf_composer::LogLevel::FATAL, "Unknown exception occurred. \n", dnf_composer::LogOutputMode::CONSOLE);
		return 1;
	}
}

