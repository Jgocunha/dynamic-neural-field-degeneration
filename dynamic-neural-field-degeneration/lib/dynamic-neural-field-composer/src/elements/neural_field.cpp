#include "elements/neural_field.h"


NeuralField::NeuralField(std::string id, const uint8_t& size, 
	const NeuralFieldParameters& parameters, 
	const ActivationFunctionParameters& activationFunctionParameters)
	: parameters(parameters), activationFunctionParameters(activationFunctionParameters)
{
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

	if (*std::max_element(components["output"].begin(), components["output"].end()) > 0)
	{
		double integral = 0.0;
		double norm = 0.0;

		// x positions array
		std::vector<double> x(size);
		for (int i = 0; i < size; i++)
			x[i] = i;

		// Compute the integral and normalization factor
		for (int i = 0; i < size - 1; i++)
		{
			double dx = x[i + 1] - x[i];
			double a = components["output"][i];
			double b = components["output"][i + 1];
			integral += (a + b) * 0.5 * dx * x[i];
			norm += (a + b) * 0.5 * dx;
		}

		// Compute the centroid
		centroid = (integral / norm) + 0.5;
	}

	return centroid;
}