#include "elements/neural_field.h"


NeuralField::NeuralField(std::string id, const int& size,
	const NeuralFieldParameters& parameters, 
	const ActivationFunctionParameters& activationFunctionParameters)
	: parameters(parameters), activationFunctionParameters(activationFunctionParameters)
{
	// Assert that the size is positive
	assert(size > 0);

	this->label = ElementLabel::NEURAL_FIELD;
	this->uniqueIdentifier = id;
	this->size = size;

	components["input"] = std::vector<double>(size);
	components["activation"] = std::vector<double>(size);
	components["output"] = std::vector<double>(size);
	components["resting level"] = std::vector<double>(size);
}

void NeuralField::init()
{
	std::fill(components["activation"].begin(), components["activation"].end(), parameters.startingRestingLevel);
	std::fill(components["input"].begin(), components["input"].end(), 0);
	std::fill(components["resting level"].begin(), components["resting level"].end(), parameters.startingRestingLevel);
	calculateOutput();
}

void NeuralField::step(const double& t, const double& deltaT)
{
	updateInput();
	calculateActivation(t, deltaT);
	calculateOutput();
}

void NeuralField::close()
{
}

void NeuralField::setParameters(const NeuralFieldParameters& parameters)
{
	this->parameters = parameters;
	init();
}

NeuralFieldParameters NeuralField::getParameters()
{
	return parameters;
}

NeuralField::~NeuralField()
{
	// no cleanup required
}

void NeuralField::calculateActivation(const double& t, const double& deltaT)
{
	for (int i = 0; i < size; i++)
	{
		components["activation"][i] = components["activation"][i] + deltaT / parameters.tau *
			(-components["activation"][i] + components["resting level"][i] + components["input"][i]);
	}
}

void NeuralField::calculateOutput()
{
	switch (activationFunctionParameters.type)
	{
		case ActivationFunctionType::Sigmoid:
			components["output"] = mathtools::sigmoid(components["activation"], 
				activationFunctionParameters.steepness, activationFunctionParameters.xShift);
			break;
		case ActivationFunctionType::Heaviside:
			components["output"] = mathtools::heaviside(components["activation"], activationFunctionParameters.xShift);
			break;
		default:
			// this should never happenl; return exception
			break;
	}
}

double NeuralField::calculateCentroid()
{

	double centroid = 0.0;
	const std::vector<double> f_output = mathtools::heaviside(components["activation"], 0.1);

	if (*std::max_element(f_output.begin(), f_output.end()) > 0)
	{
		const bool isAtLimits = (f_output[0] > 0) || (f_output[size - 1] > 0);

		double sumActivation = 0.0;
		double sumWeightedPositions = 0.0;

		for (int i = 0; i < size; i++)
		{
			const double activation = f_output[i];

			sumActivation += activation;

			// Calculate the circular distance from the midpoint to the position
			double distance = 0.0;
			if (isAtLimits)
				distance = fmod(static_cast<double>(i) - static_cast<double>(size) * 0.5 + static_cast<double>(size) * 10, static_cast<double>(size));
			else
				distance = fmod(static_cast<double>(i) - static_cast<double>(size) * 0.5, static_cast<double>(size));
			sumWeightedPositions += distance * activation;
		}

		if (sumActivation != 0.0)
		{
			// Shift the centroid back to the circular field
			centroid = fmod(static_cast<double>(size) * 0.5 + sumWeightedPositions / sumActivation, static_cast<double>(size));
			if (isAtLimits)
				centroid = (centroid >= 0 ? centroid : centroid + static_cast<double>(size));
		}
	}
	else
		centroid = -1.0;

	return centroid;
}