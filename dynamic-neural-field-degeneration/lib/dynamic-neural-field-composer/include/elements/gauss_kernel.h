#pragma once

#include <array>

#include "mathtools/mathtools.h"
#include "element.h"

struct GaussKernelParameters
{
	double sigma;
	double amplitude;
	double amplitudeGlobal;
	double fullSum;
	int cutOfFactor = 5;
	
	bool operator==(const GaussKernelParameters& other) const
	{
		return sigma == other.sigma &&
			amplitude == other.amplitude &&
			amplitudeGlobal == other.amplitudeGlobal &&
			fullSum == other.fullSum &&
			cutOfFactor == other.cutOfFactor;
	}
};

class GaussKernel : public Element
{
private:
	GaussKernelParameters parameters;
	bool circular;
	bool normalized;
	std::array<uint32_t, 2> kernelRange;
	std::vector<uint32_t> extIndex;
	bool degenerate;
public:
	GaussKernel(const std::string& id, const int& size,
		const GaussKernelParameters& parameters,
		bool circular = true, bool normalized = true);

	void init() override;
	void step(const double& t, const double& deltaT) override;
	void close() override;

	void setParameters(const GaussKernelParameters& parameters);
	GaussKernelParameters getParameters();

	~GaussKernel();
};