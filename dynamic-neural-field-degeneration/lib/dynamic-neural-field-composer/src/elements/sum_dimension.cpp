#include "./elements/sum_dimension.h"

SumDimension::SumDimension(const std::string& id, const uint8_t size,
	const SumDimensionParameters& parameters)
	: parameters(parameters)
{
	this->label = ElementLabel::SUM_DIMENSION;
	this->uniqueIdentifier = id;
	this->size = size;
	this->parameters.dimensionOrder = parameters.sumDimensions; // to do
	components["input"] = std::vector<double>(size);
	components["output"] = std::vector<double>(size);
	// to do! dimension verification
}


void SumDimension::init()
{
	std::fill(components["output"].begin(), components["output"].end(), 0);
	std::fill(components["input"].begin(), components["input"].end(), 0);
}

void SumDimension::step(const double& t, const double& deltaT)
{
	updateInput();

	double tmpSum = 0.0;

	// Compute the sum along the first dimension
	for (int i = 0; i < components["input"].size(); i++) {
		tmpSum += components["input"][i];
	}

	// Compute the sum along the remaining dimensions
	// to do (we have been working with one dimension)

	// Compute the output
	// to do (we have been working with one dimension)
	for (int i = 0; i < components["output"].size(); i++) 
		components["output"][i] = parameters.amplitude * tmpSum;
}

void SumDimension::close()
{
}

void SumDimension::setParameters(const SumDimensionParameters& parameters)
{
	this->parameters = parameters;
	init();
}

SumDimensionParameters SumDimension::getParameters()
{
	return parameters;
}

