#include "elements/gauss_kernel.h"

GaussKernel::GaussKernel(const std::string& id, const int& size,
	const GaussKernelParameters& parameters,
	bool circular, bool normalized)
	: parameters(parameters),
	circular(circular), normalized(normalized)
{
	// Assert that the size is positive
	assert(size > 0);

	this->label = ElementLabel::GAUSS_KERNEL;
	this->uniqueIdentifier = id;
	this->size = size;
	components["kernel"] = std::vector<double>(size);
	components["output"] = std::vector<double>(size);
	components["input"] = std::vector<double>(size);
}

void GaussKernel::init()
{
	kernelRange = mathtools::computeKernelRange(parameters.sigma, parameters.cutOfFactor, size, circular);

	if (circular)
		extIndex = mathtools::createExtendedIndex(size, kernelRange);
	else
		extIndex = {};

	uint32_t rangeXsize = kernelRange[0] + kernelRange[1] + 1;
	std::vector<int> rangeX(rangeXsize);
	int startingValue = kernelRange[0];
	std::iota(rangeX.begin(), rangeX.end(), -startingValue);
	std::vector<double> gauss(size);
	if (normalized)
		gauss = mathtools::gaussNorm(rangeX, 0.0, parameters.sigma);
	else
		gauss = mathtools::gauss(rangeX, 0.0, parameters.sigma);

	components["kernel"].resize(rangeX.size());
	for (int i = 0; i < components["kernel"].size(); i++)
		components["kernel"][i] = parameters.amplitude * gauss[i];

	components["input"].resize(extIndex.size());
	parameters.fullSum = 0;
	std::fill(components["input"].begin(), components["input"].end(), 0);
}

void GaussKernel::step(const double& t, const double& deltaT)
{

	updateInput();

	parameters.fullSum = std::accumulate(components["input"].begin(), components["input"].end(), (double)0.0);

	std::vector<double> convolution(size);
	std::vector<double> subDataInput = mathtools::obtainCircularVector(extIndex, components["input"]);


	if (circular)
		convolution = mathtools::conv_valid(subDataInput, components["kernel"]);
	else
		convolution = mathtools::conv(subDataInput, components["kernel"]);

	//components["output"] = convolution;
	for (int i = 0; i < components["output"].size(); i++)
		components["output"][i] = convolution[i] + parameters.amplitudeGlobal * parameters.fullSum;
}

void GaussKernel::close()
{
}


void GaussKernel::setParameters(const GaussKernelParameters& parameters)
{
	this->parameters = parameters;
	init();
}


GaussKernelParameters GaussKernel::getParameters()
{
	return parameters;
}

GaussKernel::~GaussKernel()
{
	// nothing requires cleanup
}

