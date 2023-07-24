#include "elements/gauss_stimulus.h"

GaussStimulus::GaussStimulus(std::string id, const int& size, const GaussStimulusParameters& parameters)
	: parameters(parameters)
{
	// Assert that the size is positive
	assert(size > 0);

	if (parameters.position < 0 || parameters.position >= size)
		throw Exception(ErrorCode::GAUSS_STIMULUS_POSITION_OUT_OF_RANGE, id);

	this->label = ElementLabel::GAUSS_STIMULUS;
	this->uniqueIdentifier = id;
	this->size = size;
	components["output"] = std::vector<double>(size);
	this->parameters.circular = true;
	this->parameters.normalized = false;
}

void GaussStimulus::init()
{
	std::vector<double> g(size);

	if (parameters.circular)
		g = mathtools::circularGauss(size, parameters.sigma, parameters.position);
	else
		std::cout << "Not implemented yet" << std::endl;

	if(!parameters.normalized)
		for (int i = 0; i < size; i++)
			components["output"][i] = parameters.amplitude * g[i];
	else
		std::cout << "Not implemented yet" << std::endl;

	// if stimulus is being used as a sum of stimulus
	components["input"] = std::vector<double>(size);
	updateInput();
	for (int i = 0; i < size; i++)
		components["output"][i] += components["input"][i];
}

void GaussStimulus::step(const double& t, const double& deltaT)
{
}

void GaussStimulus::close()
{
}

void GaussStimulus::setParameters(const GaussStimulusParameters& parameters)
{
	this->parameters = parameters;
	init();
}

GaussStimulusParameters GaussStimulus::getParameters()
{
	return parameters;
}

GaussStimulus GaussStimulus::operator+(const GaussStimulus& other) const
{
	GaussStimulus result(*this); // Create a copy of the current object
	if (components.count("output") > 0 && other.components.count("output") > 0)
	{
		const std::vector<double> output = other.components.at("output"); // Get the output vector from the other object
		std::vector<double> resultOutput = result.components["output"]; // Get the output vector from the result object
		if (output.size() == resultOutput.size())
		{
			// Sum the output vectors element-wise
			for (size_t i = 0; i < resultOutput.size(); i++)
			{
				resultOutput[i] += output[i];
			}
		}
		else
			throw Exception(ErrorCode::GAUSS_STIMULUS_SUM_MISMATCH, other.uniqueIdentifier);
	}

	return result;
}


std::shared_ptr<GaussStimulus> GaussStimulus::operator+(const std::shared_ptr<GaussStimulus>& other) const
{
	std::shared_ptr<GaussStimulus> result = std::make_shared<GaussStimulus>(*this); // Create a copy of the current object
	if (components.count("output") > 0 && other->components.count("output") > 0)
	{
		const std::vector<double>& output = other->components.at("output"); // Get the output vector from the other object
		std::vector<double>& resultOutput = result->components["output"]; // Get the output vector from the result object
		if (output.size() == resultOutput.size())
		{
			// Sum the output vectors element-wise
			for (size_t i = 0; i < resultOutput.size(); i++)
			{
				resultOutput[i] += output[i];
			}
		}
		else
			throw Exception(ErrorCode::GAUSS_STIMULUS_SUM_MISMATCH, other->uniqueIdentifier);
	}
	return result;
}



GaussStimulus::~GaussStimulus()
{
	// nothing requires cleanup
}
