#include "elements/normal_noise.h"

NormalNoise::NormalNoise(std::string id, const int& size, const NormalNoiseParameters& parameters)
	: parameters(parameters)
{
	// Assert that the size is positive
	assert(size > 0);

	this->label = ElementLabel::NORMAL_NOISE;
	this->uniqueIdentifier = id;
	this->size = size;
	components["output"] = std::vector<double>(size);
}

void NormalNoise::init()
{
	std::fill(components["output"].begin(), components["output"].end(), 0);
}

void NormalNoise::step(const double& t, const double& deltaT)
{
	std::vector<double> rand = mathtools::generateNormalVector(size);

	for (int i = 0; i < size; i++)
		components["output"][i] = parameters.amplitude / sqrt(deltaT) * rand[i];
}

void NormalNoise::setParameters(const NormalNoiseParameters& parameters)
{
	this->parameters = parameters;
}

NormalNoiseParameters NormalNoise::getParameters()
{
	return parameters;
}

NormalNoise::~NormalNoise()
{
	// nothing requires cleanup 
}