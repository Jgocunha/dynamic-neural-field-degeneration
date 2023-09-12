#include "./elements/degenerate_field_coupling.h"

DegenerateFieldCoupling::DegenerateFieldCoupling(const std::string& id, const int& outputSize, const int& inputSize, const FieldCouplingParameters& parameters, const LearningRule& learningRule)
	: FieldCoupling(id, outputSize, inputSize, parameters, learningRule)
{
	// Assert that the sizes are positive
	assert(outputSize > 0 && inputSize > 0);

	degeneracyType = ElementDegeneracyType::NONE;
	degenerate = false;
	populateIndicesForDegeneration();
}

void DegenerateFieldCoupling::init()
{
	FieldCoupling::init();
	findMinMaxWeightValues();
	degenerate = false;
}

void DegenerateFieldCoupling::step(const double& t, const double& deltaT)
{
	FieldCoupling::step(t, deltaT);
	if (degenerate)
		applyDegeneracy();
}

void DegenerateFieldCoupling::startDegeneration()
{
	degenerate = true;
}

void DegenerateFieldCoupling::updateWeights(const std::vector<double>& input, const std::vector<double>& output)
{
	if(!updateAllWeights)
	{
		std::cout << "Updating only degenerated weights!" << std::endl;
		weights = learningRuleDegenerate(input, output);
	}
	else
	{
		std::cout << "Updating all weights!" << std::endl;
		weights = mathtools::deltaLearningRuleKroghHertz(weights, input, output, parameters.learningRate);
	}

	writeWeights();
}

void DegenerateFieldCoupling::applyDegeneracy()
{
	double percentage = 0.1; // 1 percent
	//double numWeightsToDegenerate = (components["output"].size() * components["input"].size()) * percentage;
	constexpr double numWeightsToDegenerate = 100;

	std::cout << "Indices for degeneration size: " << indicesForDegeneration.size() << std::endl;
	switch (degeneracyType)
	{
	case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
		for (int i = 0; i < numWeightsToDegenerate; i++)
			setRandomUniqueWeightToZero();
		degenerate = false;
		break;
	case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
		for (int i = 0; i < numWeightsToDegenerate; i++)
			setRandomWeightToRandomValue();
		degenerate = false;
		break;
	case ElementDegeneracyType::WEIGHTS_REDUCE:
		for (int i = 0; i < numWeightsToDegenerate; i++)
			setRandomWeightToReduceValue();
		degenerate = false;
		break;
	default:
		std::cout << "Degeneracy type not supported" << std::endl;
		break;
	}
	std::cout << "After degeneration indices size: " << indicesForDegeneration.size() << std::endl;

}

void DegenerateFieldCoupling::setDegeneracyType(ElementDegeneracyType degeneracyType)
{
	this->degeneracyType = degeneracyType;
}

ElementDegeneracyType DegenerateFieldCoupling::getDegeneracyType()
{
	return degeneracyType;
}

void DegenerateFieldCoupling::populateIndicesForDegeneration()
{
	for (int i = 0; i < components["output"].size(); i++)
	{
		for (int j = 0; j < components["input"].size(); j++)
		{
			std::pair<int, int> pair(j, i);
			indicesForDegeneration.insert(pair);
		}
	}
}

void DegenerateFieldCoupling::findMinMaxWeightValues()
{
	// Find the minimum and maximum values of the weights
	for (int i = 0; i < components["output"].size(); i++)
	{
		for (int j = 0; j < components["input"].size(); j++)
		{
			minWeightValue = std::min(minWeightValue, weights[j][i]);
			maxWeightValue = std::max(maxWeightValue, weights[j][i]);
		}
	}
}

void DegenerateFieldCoupling::setRandomWeightToRandomValue()
{
	const int row_idx = mathtools::generateRandomNumber(0, static_cast<int>(components["input"].size()) - 1);
	const int col_idx = mathtools::generateRandomNumber(0, static_cast<int>(components["output"].size()) - 1);

	const double newValue = mathtools::generateRandomNumber(minWeightValue, maxWeightValue);
	weights[row_idx][col_idx] = newValue;
}

void DegenerateFieldCoupling::setWeightReductionFactor(const double& factor)
{
	weightReductionFactor = factor;
}

double DegenerateFieldCoupling::getWeightReductionFactor()
{
	return weightReductionFactor;
}

void DegenerateFieldCoupling::setRandomWeightToReduceValue()
{
	while (true)
	{
		const int row_idx = mathtools::generateRandomNumber(0, static_cast<int>(components["input"].size()) - 1);
		const int col_idx = mathtools::generateRandomNumber(0, static_cast<int>(components["output"].size()) - 1);
		if (weights[row_idx][col_idx] != 0.0)
		{
			weights[row_idx][col_idx] = weights[row_idx][col_idx] * weightReductionFactor;
			break;
		}
	}
}

void DegenerateFieldCoupling::setRandomUniqueWeightToZero()
{
	// Initialize the maximum number of attempts to find unique combinations
	//static int maxAttempts = static_cast<int>(components["output"].size()) * static_cast<int>(components["input"].size());

	bool uniqueCombinationFound = false; // Flag to indicate if a unique combination is found

	// Loop until a unique combination is found or indicesForDegeneration is empty
	while (!uniqueCombinationFound && !indicesForDegeneration.empty())
	{
		int size = indicesForDegeneration.size() - 1;
		// Get a random iterator from the set
		auto randomIterator = std::next(indicesForDegeneration.begin(), mathtools::generateRandomNumber(0, size));

		// Get the pair from the iterator
		const std::pair<int, int> pair = *randomIterator;

		// Erase the combination from the set
		indicesForDegeneration.erase(randomIterator);

		const int row_idx = pair.first;
		const int col_idx = pair.second;

		// Set weight at combination to 0
		weights[row_idx][col_idx] = 0;

		uniqueCombinationFound = true; // Set flag to indicate combination found
		//std::cout << "Unique combination found " << row_idx << " " << col_idx << std::endl;
	}

	if (indicesForDegeneration.empty())
		std::cout << "No more unique combinations to degenerate" << std::endl;
	//else 
		//std::cout << "Attempting to find unique combination" << std::endl;
}

std::vector<std::vector<double>> DegenerateFieldCoupling::learningRuleDegenerate(
	const std::vector<double>& input, const std::vector<double>& targetOutput)
{

	double deltaT = 1.0;
	double tau_w = 5.0;
	double eta = 0.5;

	int containCount = 0;
	int notContainCount = 0;

	const size_t inputSize = input.size();
	const size_t outputSize = targetOutput.size(); //fixed from int to size_t

	// Calculate the activation levels of the fields based on the input values and current weights
	std::vector<double> actualOutput(outputSize, 0.0);
	for (size_t j = 0; j < outputSize; ++j) {
		for (size_t i = 0; i < inputSize; ++i) {
			actualOutput[j] += input[i] * weights[i][j];
		}
	}

	// Calculate the error between the target output and the actual output
	std::vector<double> error(outputSize, 0.0);
	for (size_t j = 0; j < outputSize; ++j) {
		error[j] = targetOutput[j] - actualOutput[j];
	}

	// Update the weights based on the error and current activation levels of the fields
	for (size_t i = 0; i < inputSize; ++i) {
		for (size_t j = 0; j < outputSize; ++j)
		{
			// If the pair is found in the set (then it still hasn't degenerated), then update the weight.
			if(indicesForDegeneration.contains(std::make_pair(i, j)))
			{
				weights[i][j] += parameters.learningRate * (error[j] - eta * weights[i][j]) * input[i];
				containCount++;

			}
			else
			{
				notContainCount++;
			}
			// else do nothing
		}
	}

	std::cout << "Contain count: " << containCount << std::endl;
	std::cout << "Not contain count: " << notContainCount << std::endl;

	return weights;
}