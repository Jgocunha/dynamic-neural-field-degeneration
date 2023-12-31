#include "./elements/degenerate_field_coupling.h"

DegenerateFieldCoupling::DegenerateFieldCoupling(const std::string& id, const int& outputSize, const int& inputSize, const FieldCouplingParameters& parameters, const LearningRule& learningRule)
	: FieldCoupling(id, outputSize, inputSize, parameters, learningRule)
{
	// Assert that the sizes are positive
	assert(outputSize > 0 && inputSize > 0);

	degeneracyType = ElementDegeneracyType::NONE;
	degenerate = false;
}

void DegenerateFieldCoupling::init()
{
	FieldCoupling::init();
	populateIndicesForDegeneration();
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

void DegenerateFieldCoupling::updateWeights(const std::vector<double> input, const std::vector<double> output)
{
	weights = learningRuleDegenerate(weights, input, output, parameters.learningRate);
	writeWeights();
}

void DegenerateFieldCoupling::applyDegeneracy()
{
	double percentage = 0.1; // 1 percent
	//double numWeightsToDegenerate = (components["output"].size() * components["input"].size()) * percentage;
	double numWeightsToDegenerate = 648;
	switch (degeneracyType)
	{
		case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
			setRandomUniqueWeightToZero();
			degenerate = false;
			break;
		case ElementDegeneracyType::WEIGHTS_DEACTIVATE_PERCENTAGE:
			while (numWeightsToDegenerate > 0)
			{
				setRandomUniqueWeightToZero();
				numWeightsToDegenerate--;
			}
			degenerate = false;
			break;
		case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
			setRandomWeightToRandomValue();
			degenerate = false;
			break;
		case ElementDegeneracyType::WEIGHTS_RANDOMIZE_PERCENTAGE:
			while (numWeightsToDegenerate > 0)
			{
				setRandomWeightToRandomValue();
				numWeightsToDegenerate--;
			}
			degenerate = false;
			break;
		case ElementDegeneracyType::WEIGHTS_REDUCE:
			setRandomWeightToReduceValue();
			degenerate = false;
			break;
		case ElementDegeneracyType::WEIGHTS_REDUCE_PERCENTAGE:
			while (numWeightsToDegenerate > 0)
			{
				setRandomWeightToReduceValue();
				numWeightsToDegenerate--;
			}
			degenerate = false;
			break;
		default:
			std::cout << "Degeneracy type not supported" << std::endl;
			break;
	}
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
	//static int maxAttempts = components["output"].size() * components["input"].size();
	int row_idx, col_idx;

	//while(1)
	//{
		row_idx = mathtools::generateRandomNumber(0, (int)components["input"].size() - 1);
		col_idx = mathtools::generateRandomNumber(0, (int)components["output"].size() - 1);
		//if (weights[row_idx][col_idx] > 0)
		//{
		double aux = mathtools::generateRandomNumber(minWeightValue, maxWeightValue);
		weights[row_idx][col_idx] = aux;
		//break;
		//}
	//}
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
	//static int maxAttempts = components["output"].size() * components["input"].size();
	int row_idx, col_idx;

	while(1)
	{
		row_idx = mathtools::generateRandomNumber(0, (int)components["input"].size() - 1);
		col_idx = mathtools::generateRandomNumber(0, (int)components["output"].size() - 1);
		if (weights[row_idx][col_idx] != 0)
		{
			weights[row_idx][col_idx] = weights[row_idx][col_idx] * weightReductionFactor;
			break;
		}
	}
}

void DegenerateFieldCoupling::setRandomUniqueWeightToZero()
{
	// Initialize the maximum number of attempts to find unique combinations
	static int maxAttempts = components["output"].size() * components["input"].size();

	int row_idx, col_idx; // Indices for rows and columns
	bool uniqueCombinationFound = false; // Flag to indicate if a unique combination is found
	int numIndices = static_cast<int>((components["output"].size() * components["input"].size()) / 10.0);

	// Loop until a unique combination is found or indicesForDegeneration is empty
	while (!uniqueCombinationFound)
	{
		// Generate random row and column indices
		row_idx = mathtools::generateRandomNumber(0, (int)components["input"].size() - 1);
		col_idx = mathtools::generateRandomNumber(0, (int)components["output"].size() - 1);

		// Create a pair of indices representing a combination
		std::pair<int, int> pair(row_idx, col_idx);

		// Check if the combination is in the set of indices for degeneration
		if (indicesForDegeneration.find(pair) != indicesForDegeneration.end())
		{
			// If combination found, update data and exit loop
			indicesForDegeneration.erase(pair); // Remove combination from set
			weights[row_idx][col_idx] = 0;      // Set weight at combination to 0
			uniqueCombinationFound = true;      // Set flag to indicate combination found
		}
		// Check if all desired combinations have been found and processed
		else if (!(indicesForDegeneration.size()))
		{
			uniqueCombinationFound = true; // Set flag to exit loop
		}
	}
}

//void DegenerateFieldCoupling::setRandomUniqueWeightToZero()
//{
//	static int maxAttempts = components["output"].size() * components["input"].size();
//
//	std::vector<std::pair<int, int>> validCombinations;
//	for (int row_idx = 0; row_idx < components["input"].size(); ++row_idx)
//	{
//		for (int col_idx = 0; col_idx < components["output"].size(); ++col_idx)
//		{
//			std::pair<int, int> pair(row_idx, col_idx);
//			if (indicesForDegeneration.find(pair) != indicesForDegeneration.end())
//			{
//				validCombinations.push_back(pair);
//			}
//		}
//	}
//
//	// Shuffle the vector of valid combinations
//	std::random_device rd;
//	std::mt19937 gen(rd());
//	std::shuffle(validCombinations.begin(), validCombinations.end(), gen);
//
//	for (const auto& combination : validCombinations)
//	{
//		int row_idx = combination.first;
//		int col_idx = combination.second;
//
//		indicesForDegeneration.erase(combination);
//		weights[row_idx][col_idx] = 0;
//		if (!indicesForDegeneration.size())
//		{
//			break; // Exit loop if all combinations processed
//		}
//	}
//}

std::vector<std::vector<double>> DegenerateFieldCoupling::learningRuleDegenerate(std::vector<std::vector<double>>& weights,
	const std::vector<double>& input, const std::vector<double>& targetOutput, const double& learningRate)
{

	double deltaT = 1.0;
	double tau_w = 5.0;
	double eta = 0.5;

	size_t inputSize = input.size();
	size_t outputSize = targetOutput.size(); //fixed from int to size_t

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
			std::pair<int, int> pair(i, j);
			auto it = std::find(indicesForDegeneration.begin(), indicesForDegeneration.end(), pair);

			// If the pair is found in the set (then it still hasn't degenerated), then update the weight.
			//if (it != indicesForDegeneration.end())
			//{
				weights[i][j] += learningRate * (error[j] - eta * weights[i][j]) * input[i];
			//}
			// else do nothing
		}
	}

	return weights;
}