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

void DegenerateFieldCoupling::applyDegeneracy()
{
	double percentage = 0.1;
	double numWeightsToDegenerate = (size*size) * percentage;
	switch (degeneracyType)
	{
		case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
			setRandomUniqueWeightToZero();
			break;
		case ElementDegeneracyType::WEIGHTS_DEACTIVATE_PERCENTAGE:
			while (numWeightsToDegenerate)
			{
				setRandomUniqueWeightToZero();
				numWeightsToDegenerate--;
			}
			degenerate = false;
			break;
		case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
			setRandomWeightToRandomValue();
			break;
		case ElementDegeneracyType::WEIGHTS_REDUCE:
			setRandomWeightToReduceValue();
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
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			std::pair<int, int> pair(i, j);
			indicesForDegeneration.insert(pair);
		}
	}
}

void DegenerateFieldCoupling::findMinMaxWeightValues()
{
	// Find the minimum and maximum values of the weights
	for (int row = 0; row < size; row++)
	{
		for (int col = 0; col < size; col++)
		{
			minWeightValue = std::min(minWeightValue, weights[row][col]);
			maxWeightValue = std::max(maxWeightValue, weights[row][col]);
		}
	}
}

void DegenerateFieldCoupling::setRandomWeightToRandomValue()
{
	int maxAttempts = size * size;
	int row_idx, col_idx;

	for (int i = 0; i < maxAttempts; i++)
	{
		row_idx = mathtools::generateRandomNumber(0, size - 1);
		col_idx = mathtools::generateRandomNumber(0, size - 1);
		if (weights[row_idx][col_idx] > 0)
		{
			weights[row_idx][col_idx] = mathtools::generateRandomNumber(minWeightValue, maxWeightValue);
			break;
		}
	}
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
	int maxAttempts = size * size;
	int row_idx, col_idx;
	for (int i = 0; i < maxAttempts; i++)
	{
		row_idx = mathtools::generateRandomNumber(0, size - 1);
		col_idx = mathtools::generateRandomNumber(0, size - 1);
		if (weights[row_idx][col_idx] > 0)
		{
			weights[row_idx][col_idx] = weights[row_idx][col_idx] * weightReductionFactor;
			break;
		}
	}
}

void DegenerateFieldCoupling::setRandomUniqueWeightToZero()
{
	int maxAttempts = size * size;
	int row_idx, col_idx;
	bool uniqueCombinationFound = false;
	int numIndices = static_cast<int>((size * size) / 10.0);
	while (!uniqueCombinationFound)
	{
		row_idx = mathtools::generateRandomNumber(0, size - 1);
		col_idx = mathtools::generateRandomNumber(0, size - 1);
		std::pair<int, int> pair(row_idx, col_idx);
		if (indicesForDegeneration.find(pair) != indicesForDegeneration.end())
		{
			indicesForDegeneration.erase(pair);
			weights[row_idx][col_idx] = 0;
			uniqueCombinationFound = true;
		}
		else if (!(indicesForDegeneration.size()))
		{
			uniqueCombinationFound = true;
		}
	}
}
