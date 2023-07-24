
#pragma once

#include "mathtools/mathtools.h"

#include "element.h"
 
struct NormalNoiseParameters
{
	double amplitude;
	bool operator==(const NormalNoiseParameters& other) const
	{
		return amplitude == other.amplitude;
	}
};

class NormalNoise : public Element
{
private:
	NormalNoiseParameters parameters;
public:
	NormalNoise(std::string id, const int& size, const NormalNoiseParameters& parameters);

	void init() override;
	void step(const double& t, const double& deltaT) override;
	void close() override {}

	void setParameters(const NormalNoiseParameters& parameters);
	NormalNoiseParameters getParameters();

	~NormalNoise();
};