#include "degenerate_field_coupling.h"

DegenerateFieldCoupling::DegenerateFieldCoupling(const dnf_composer::element::ElementCommonParameters& elementCommonParameters,
	const dnf_composer::element::FieldCouplingParameters& parameters)
	: FieldCoupling(elementCommonParameters, parameters)
{
	degeneracyType = experiment::degeneration::ElementDegeneracyType::NONE;
	degenerate = false;
	//populateIndicesForDegeneration(); // for recovering from degeneration experiment
}

void DegenerateFieldCoupling::init()
{
	FieldCoupling::init();
	populateIndicesForDegeneration(); // uncomment for inducing degeneration experiment
	findMinMaxWeightValues();
	degenerate = false;
}

void DegenerateFieldCoupling::step(double t, double deltaT)
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
	weights = learningRuleDegenerate(weights, input, output, parameters.learningRate);
	//writeWeights();
}

void DegenerateFieldCoupling::setNumWeightsToDegenerate(int count)
{
	numWeightsToDegenerate = count;
}

void DegenerateFieldCoupling::applyDegeneracy()
{
	switch (degeneracyType)
	{
	case experiment::degeneration::ElementDegeneracyType::WEIGHTS_DEACTIVATE:
		for (int i = 0; i < numWeightsToDegenerate; i++)
			setRandomUniqueWeightToZero();
		degenerate = false;
		break;
	case experiment::degeneration::ElementDegeneracyType::WEIGHTS_RANDOMIZE:
		for (int i = 0; i < numWeightsToDegenerate; i++)
			//setRandomWeightToRandomValue();
			setRandomUniqueWeightToRandomValue();
		degenerate = false;
		break;
	case experiment::degeneration::ElementDegeneracyType::WEIGHTS_REDUCE:
		for (int i = 0; i < numWeightsToDegenerate; i++)
			//setRandomWeightToReduceValue();
			setRandomUniqueWeightToReduceValue();
		degenerate = false;
		break;
	default:
		log(dnf_composer::tools::logger::LogLevel::ERROR, "Degeneracy type not supported");
		break;
	}
}

int DegenerateFieldCoupling::getNumIndicesForDegeneration() const
{
	return static_cast<int>(indicesForDegeneration.size());
}

void DegenerateFieldCoupling::setDegeneracyType(experiment::degeneration::ElementDegeneracyType degeneracyType)
{
	this->degeneracyType = degeneracyType;
}

experiment::degeneration::ElementDegeneracyType DegenerateFieldCoupling::getDegeneracyType() const
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
			maxWeightValue = std::max(maxWeightValue, weights[j][i]) - 0.001;// -0.0055;
		}
	}
}

void DegenerateFieldCoupling::setRandomWeightToRandomValue()
{
	//static int maxAttempts = components["output"].size() * components["input"].size();
	int row_idx, col_idx;

	//while(1)
	//{
	row_idx = dnf_composer::tools::utils::generateRandomNumber(0, (int)components["input"].size() - 1);
	col_idx = dnf_composer::tools::utils::generateRandomNumber(0, (int)components["output"].size() - 1);
	//if (weights[row_idx][col_idx] > 0)
	//{
	double aux = dnf_composer::tools::utils::generateRandomNumber(minWeightValue, maxWeightValue);
	weights[row_idx][col_idx] = aux;
	//break;
	//}
//}
}

void DegenerateFieldCoupling::setWeightReductionFactor(const double& factor)
{
	weightReductionFactor = factor;
}

double DegenerateFieldCoupling::getWeightReductionFactor() const
{
	return weightReductionFactor;
}

void DegenerateFieldCoupling::setRandomWeightToReduceValue()
{
	//static int maxAttempts = components["output"].size() * components["input"].size();
	int row_idx, col_idx;

	while (true)
	{
		row_idx = dnf_composer::tools::utils::generateRandomNumber(0, (int)components["input"].size() - 1);
		col_idx = dnf_composer::tools::utils::generateRandomNumber(0, (int)components["output"].size() - 1);
		if (weights[row_idx][col_idx] != 0)
		{
			weights[row_idx][col_idx] = weights[row_idx][col_idx] * weightReductionFactor;
			break;
		}
	}
}

void DegenerateFieldCoupling::setRandomUniqueWeightToRandomValue()
{
	bool uniqueCombinationFound = false; // Flag to indicate if a unique combination is found

	// Loop until a unique combination is found or indicesForDegeneration is empty
	while (!uniqueCombinationFound && !indicesForDegeneration.empty())
	{
		int size = indicesForDegeneration.size() - 1;
		// Get a random iterator from the set
		auto randomIterator = std::next(indicesForDegeneration.begin(), dnf_composer::tools::utils::generateRandomNumber(0, size));

		// Get the pair from the iterator
		const std::pair<int, int> pair = *randomIterator;

		// Erase the combination from the set
		indicesForDegeneration.erase(randomIterator);

		const int row_idx = pair.first;
		const int col_idx = pair.second;

		const double aux = dnf_composer::tools::utils::generateRandomNumber(minWeightValue, maxWeightValue);
		weights[row_idx][col_idx] = aux;

		uniqueCombinationFound = true; // Set flag to indicate combination found
		//std::cout << "Unique combination found " << row_idx << " " << col_idx << std::endl;
	}

	if (indicesForDegeneration.empty())
	{
		//std::cout << "No more unique combinations to degenerate" << std::endl;
	}
	/*else
	{
		std::cout << "Attempting to find unique combination" << std::endl;
	}*/
}

void DegenerateFieldCoupling::setRandomUniqueWeightToReduceValue()
{
	bool uniqueCombinationFound = false; // Flag to indicate if a unique combination is found

	// Loop until a unique combination is found or indicesForDegeneration is empty
	while (!uniqueCombinationFound && !indicesForDegeneration.empty())
	{
		int size = static_cast<int>(indicesForDegeneration.size()) - 1;
		// Get a random iterator from the set
		auto randomIterator = std::next(indicesForDegeneration.begin(), dnf_composer::tools::utils::generateRandomNumber(0, size));

		// Get the pair from the iterator
		const std::pair<int, int> pair = *randomIterator;

		// Erase the combination from the set
		indicesForDegeneration.erase(randomIterator);

		const int row_idx = pair.first;
		const int col_idx = pair.second;

		// Set weight at combination to 0
		weights[row_idx][col_idx] = weights[row_idx][col_idx] * weightReductionFactor;

		uniqueCombinationFound = true; // Set flag to indicate combination found
		//std::cout << "Unique combination found " << row_idx << " " << col_idx << std::endl;
		//std::cout << "Weight reduction factor " << weightReductionFactor << std::endl;
	}

	if (indicesForDegeneration.empty())
	{
		//std::cout << "No more unique combinations to degenerate" << std::endl;
	}
	/*else
	{
		std::cout << "Attempting to find unique combination" << std::endl;
	}*/
}


void DegenerateFieldCoupling::setRandomUniqueWeightToZero()
{
	bool uniqueCombinationFound = false; // Flag to indicate if a unique combination is found
	//static int count = 0;

	// Loop until a unique combination is found or indicesForDegeneration is empty
	while (!uniqueCombinationFound && !indicesForDegeneration.empty())
	{
		int size = static_cast<int>(indicesForDegeneration.size()) - 1;
		// Get a random iterator from the set
		auto randomIterator = std::next(indicesForDegeneration.begin(), dnf_composer::tools::utils::generateRandomNumber(0, size));

		// Get the pair from the iterator
		const std::pair<int, int> pair = *randomIterator;

		// Erase the combination from the set
		indicesForDegeneration.erase(randomIterator);

		const int row_idx = pair.first;
		const int col_idx = pair.second;

		// Set weight at combination to 0
		weights[row_idx][col_idx] = 0;

		uniqueCombinationFound = true; // Set flag to indicate combination found
		//count++;
		//std::cout << "Unique combination found " << row_idx << " " << col_idx << " "<< count << std::endl;
	}

	if (indicesForDegeneration.empty())
	{
		std::cout << "No more unique combinations to degenerate" << std::endl;
	}
	/*else
	{
		std::cout << "Attempting to find unique combination" << std::endl;
	}*/
}

//void DegenerateFieldCoupling::setRandomUniqueWeightToZero()
//{
//	// Initialize the maximum number of attempts to find unique combinations
//	static int maxAttempts = components["output"].size() * components["input"].size();
//
//	int row_idx, col_idx; // Indices for rows and columns
//	bool uniqueCombinationFound = false; // Flag to indicate if a unique combination is found
//	//int numIndices = static_cast<int>((components["output"].size() * components["input"].size()) / 10.0);
//
//	// Loop until a unique combination is found or indicesForDegeneration is empty
//	while (!uniqueCombinationFound)
//	{
//		// Generate random row and column indices
//		row_idx = mathtools::generateRandomNumber(0, (int)components["input"].size() - 1);
//		col_idx = mathtools::generateRandomNumber(0, (int)components["output"].size() - 1);
//
//		// Create a pair of indices representing a combination
//		std::pair<int, int> pair(row_idx, col_idx);
//
//		// Check if the combination is in the set of indices for degeneration
//		//if (indicesForDegeneration.find(pair) != indicesForDegeneration.end())
//		if (indicesForDegeneration.contains(pair))
//		{
//			// If combination found, update data and exit loop
//			indicesForDegeneration.erase(pair); // Remove combination from set
//			weights[row_idx][col_idx] = 0;      // Set weight at combination to 0
//			uniqueCombinationFound = true;      // Set flag to indicate combination found
//			std::cout << "Unique combination found " << row_idx << " " << col_idx  << std::endl;
//
//		}
//		// Check if all desired combinations have been found and processed
//		//else if (!(indicesForDegeneration.size
//		else if (indicesForDegeneration.empty())
//		{
//			uniqueCombinationFound = true; // Set flag to exit loop
//			std::cout << "No more unique combinations to degenerate" << std::endl;
//		}
//		std::cout << "Attempting to find unique combination" << std::endl;
//	}
//}

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
	const std::vector<double>& input, const std::vector<double>& targetOutput, const double& learningRate) const
{

	double deltaT = 1.0;
	double tau_w = 5.0;
	double eta = 0.5;

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
			if (updateAllWeights)
				weights[i][j] += learningRate * (error[j] - eta * weights[i][j]) * input[i];
			else
			{
				std::pair<int, int> pair(i, j);
				auto it = std::find(indicesForDegeneration.begin(), indicesForDegeneration.end(), pair);
				if (it != indicesForDegeneration.end())
				{
					weights[i][j] += learningRate * (error[j] - eta * weights[i][j]) * input[i];
				}
				// else do nothing
			}
		}
	}

	return weights;
}