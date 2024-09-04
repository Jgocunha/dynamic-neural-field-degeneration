#include "degenerate_neural_field.h"


DegenerateNeuralField::DegenerateNeuralField(const dnf_composer::element::ElementCommonParameters& elementCommonParameters,
	const dnf_composer::element::NeuralFieldParameters& parameters)
	: NeuralField(elementCommonParameters, parameters)
{
	degeneracyType = ElementDegeneracyType::NONE;
	degenerate = false;
	populateIndicesForDegeneration();
	degeneratedIndices.clear();
}

void DegenerateNeuralField::init()
{
	NeuralField::init();
	populateIndicesForDegeneration();
	degenerate = false;
}

void DegenerateNeuralField::calculateActivation(const double& t, const double& deltaT)
{
	for (int i = 0; i < commonParameters.dimensionParameters.size; i++)
	{
		auto it = std::find(degeneratedIndices.begin(), degeneratedIndices.end(), i);
		if (it != degeneratedIndices.end()) // Skip the "killed" neurons
		{
			components["activation"][i] = 0;
		}
		else
		{
			components["activation"][i] = components["activation"][i] + deltaT / parameters.tau *
				(-components["activation"][i] + components["resting level"][i] + components["input"][i]);
		}
	}
}

void DegenerateNeuralField::step(double t, double deltaT)
{
	updateInput();
	if (degenerate)
		applyDegeneracy();
	calculateActivation(t, deltaT);
	calculateOutput();
	updateState();
}

void DegenerateNeuralField::startDegeneration()
{
	degenerate = true;
}

void DegenerateNeuralField::setNumNeuronsToDegenerate(const int& numNeuronsToDegenerate)
{
	this->numNeuronsToDegenerate = numNeuronsToDegenerate;
}

void DegenerateNeuralField::applyDegeneracy()
{
	//double percentage = 0.01; // 1 percent
	//double numNeuronsToDegenerate = commonParameters.dimensionParameters.size * percentage;

	switch (degeneracyType)
	{
	case ElementDegeneracyType::NEURONS_DEACTIVATE:
		for (int i = 0; i < numNeuronsToDegenerate; i++)
			setRandomUniqueNeuronToZero();
		degenerate = false;
		break;
	/*case ElementDegeneracyType::NEURONS_DEACTIVATE_PERCENTAGE:
		while (numNeuronsToDegenerate > 0)
		{
			setRandomUniqueNeuronToZero();
			numNeuronsToDegenerate--;
		}
		degenerate = false;
		break;*/
	default:
		//std::cout << "Degeneracy type not supported" << std::endl;
		break;
	}
}

void DegenerateNeuralField::setDegeneracyType(ElementDegeneracyType degeneracyType)
{
	this->degeneracyType = degeneracyType;
}

ElementDegeneracyType DegenerateNeuralField::getDegeneracyType()
{
	return degeneracyType;
}

void DegenerateNeuralField::populateIndicesForDegeneration()
{
	for (int i = 0; i < commonParameters.dimensionParameters.size; i++)
		indicesForDegeneration.push_back(i);
}

void DegenerateNeuralField::setRandomUniqueNeuronToZero()
{
	if (indicesForDegeneration.empty())
	{
		// No indices available for degeneration
		return;
	}

	// Randomly select an index from indicesForDegeneration
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, indicesForDegeneration.size() - 1);
	const int randomIndex = indicesForDegeneration[dis(gen)];

	// Push the random index to degeneratedIndices
	degeneratedIndices.push_back(randomIndex);

	// Remove the random index from indicesForDegeneration
	indicesForDegeneration.erase(std::remove(indicesForDegeneration.begin(), indicesForDegeneration.end(), randomIndex), indicesForDegeneration.end());
}

//void DegenerateNeuralField::close()
//{
//	NeuralField::close();
//	indicesForDegeneration.clear();
//	degeneratedIndices.clear();
//	degenerate = false;
//}
