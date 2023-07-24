#include "./elements/degenerate_neural_field.h"

DegenerateNeuralField::DegenerateNeuralField(const std::string& id, const int& size, const NeuralFieldParameters& parameters
, const ActivationFunctionParameters& activationFunctionParameters)
	: NeuralField(id, size, parameters, activationFunctionParameters)
{
	// Assert that the size is positive
	assert(size > 0);

	degeneracyType = ElementDegeneracyType::NONE;
	degenerate = false;
	populateIndicesForDegeneration();
	degeneratedIndices.clear();
}

void DegenerateNeuralField::init()
{
	NeuralField::init();
	populateIndicesForDegeneration();
}

void DegenerateNeuralField::calculateActivation(const double& t, const double& deltaT)
{
	for (int i = 0; i < size; i++)
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

void DegenerateNeuralField::step(const double& t, const double& deltaT)
{
	updateInput();
	if (degenerate)
		applyDegeneracy();
	calculateActivation(t, deltaT);
	calculateOutput();
}

void DegenerateNeuralField::startDegeneration()
{
	degenerate = true;
}

void DegenerateNeuralField::applyDegeneracy()
{
	double percentage = 0.1;
	double numNeuronsToDegenerate = size * percentage;

	switch (degeneracyType)
	{
		case ElementDegeneracyType::NEURONS_DEACTIVATE:
			setRandomUniqueNeuronToZero();
			degenerate = false;
			break;
		case ElementDegeneracyType::NEURONS_DEACTIVATE_PERCENTAGE:
			while (numNeuronsToDegenerate)
			{
				setRandomUniqueNeuronToZero();
				numNeuronsToDegenerate--;
			}
			degenerate = false;
			break;
		default:
			std::cout << "Degeneracy type not supported" << std::endl;
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
	for (int i = 0; i < size; i++)
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
	int randomIndex = indicesForDegeneration[dis(gen)];

	// Push the random index to degeneratedIndices
	degeneratedIndices.push_back(randomIndex);

	// Remove the random index from indicesForDegeneration
	indicesForDegeneration.erase(std::remove(indicesForDegeneration.begin(), indicesForDegeneration.end(), randomIndex), indicesForDegeneration.end());
}

void DegenerateNeuralField::close()
{
	NeuralField::close();
	indicesForDegeneration.clear();
	degeneratedIndices.clear();
	degenerate = false;
}

