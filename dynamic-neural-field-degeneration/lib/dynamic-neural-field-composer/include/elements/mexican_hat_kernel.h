#pragma once

#include "mathtools/mathtools.h"
#include <vector>
#include <string>
#include <array>

#include "element.h"

struct MexicanHatKernelParameters
{
	double sigmaExc;
	double amplitudeExc;
	double sigmaInh;
	double amplitudeInh;
	double amplitudeGlobal;
	double fullSum;
	int cutOfFactor;

	bool operator==(const MexicanHatKernelParameters& other) const
	{
		return sigmaExc == other.sigmaExc &&
			amplitudeExc == other.amplitudeExc &&
			sigmaInh == other.sigmaInh && 
			amplitudeInh == other.amplitudeInh &&
			amplitudeGlobal == other.amplitudeGlobal &&
			fullSum == other.fullSum &&
			cutOfFactor == other.cutOfFactor;
	}
};

class MexicanHatKernel : public Element
{
private:
	MexicanHatKernelParameters parameters;
	bool circular;
	bool normalized;
	std::array<uint32_t, 2> kernelRange;
	std::vector<uint32_t> extIndex;
public:
	MexicanHatKernel(const std::string& id, const int& size,
		const MexicanHatKernelParameters& parameters,
		bool circular = true, bool normalized = true);

	void init() override;
	void step(const double& t, const double& deltaT) override;
	void close() override;

	void setParameters(const MexicanHatKernelParameters& parameters);
	MexicanHatKernelParameters getParameters();

	~MexicanHatKernel();
};