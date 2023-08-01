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

void DegenerateFieldCoupling::applyDegeneracy()
{
	double percentage = 0.1;
	double numWeightsToDegenerate = (components["output"].size() * components["input"].size()) * percentage;
	switch (degeneracyType)
	{
		case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
			setRandomUniqueWeightToZero();
			degenerate = false;
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
			degenerate = false;
			break;
		case ElementDegeneracyType::WEIGHTS_REDUCE:
			setRandomWeightToReduceValue();
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
	static int maxAttempts = components["output"].size() * components["input"].size();
	int row_idx, col_idx;

	for (int i = 0; i < maxAttempts; i++)
	{
		row_idx = mathtools::generateRandomNumber(0, (int)components["input"].size() - 1);
		col_idx = mathtools::generateRandomNumber(0, (int)components["output"].size() - 1);
		if (weights[row_idx][col_idx] > 0)
		{
			double aux = mathtools::generateRandomNumber(minWeightValue, maxWeightValue);
			weights[row_idx][col_idx] = aux;
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
	static int maxAttempts = components["output"].size() * components["input"].size();
	int row_idx, col_idx;

	for (int i = 0; i < maxAttempts; i++)
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
	static int maxAttempts = components["output"].size() * components["input"].size();
	int row_idx, col_idx;
	bool uniqueCombinationFound = false;
	int numIndices = static_cast<int>((components["output"].size() * components["input"].size()) / 10.0);
	while (!uniqueCombinationFound)
	{
		row_idx = mathtools::generateRandomNumber(0, (int)components["input"].size() - 1);
		col_idx = mathtools::generateRandomNumber(0, (int)components["output"].size() - 1);
		std::pair<int, int> pair(row_idx, col_idx);
		if (indicesForDegeneration.find(pair) != indicesForDegeneration.end())
		{
			indicesForDegeneration.erase(pair);
			weights[row_idx][col_idx] = 0;
			uniqueCombinationFound = true;
			std::cout << "Weight deactivated " << row_idx << " " << col_idx << std::endl;
		}
		else if (!(indicesForDegeneration.size()))
		{
			uniqueCombinationFound = true;
		}
	}
}
