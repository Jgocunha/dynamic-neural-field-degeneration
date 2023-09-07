
#include "dynamic-neural-field-degeneration.h"

ExperimentParameters setExperimentParameters()
{
    ExperimentParameters params;

	params.numberOfTrials = 10;
    params.decisionTolerance = 0.5;

    params.degeneracyType = ElementDegeneracyType::WEIGHTS_REDUCE;
    params.fieldToDegenerate = "decision";
    setDegeneracyNameAndTypeOfElements(params);

    params.isDataSavingOn = false;
    params.isVisualisationOn = true;
    params.isDebugModeOn = true;

    return params;
}

void setDegeneracyNameAndTypeOfElements(ExperimentParameters& params)
{
    switch(params.degeneracyType)
    {
		case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
    		params.degeneracyName = "deactivate";
            params.typeOfElementsDegenerated = "weights";
    		break;
	    case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
    		params.degeneracyName = "randomize";
            params.typeOfElementsDegenerated = "weights";
    		break;
        case ElementDegeneracyType::WEIGHTS_REDUCE:
            params.degeneracyName = "reduce 0.4";
            params.typeOfElementsDegenerated = "weights";
            break;
	    case ElementDegeneracyType::NEURONS_DEACTIVATE:
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

