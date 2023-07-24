#pragma once

#include "element.h"


struct SumDimensionParameters
{
	uint8_t sumDimensions;
	uint8_t dimensionOrder;
	double amplitude;

	// Overload the == operator
	bool operator==(const SumDimensionParameters& other) const
	{
		return sumDimensions == other.sumDimensions &&
			dimensionOrder == other.dimensionOrder &&
			amplitude == other.amplitude;
	}
};

class SumDimension : public Element
{
private:
	SumDimensionParameters parameters;
public:
	SumDimension(const std::string& id, const int& size,
				const SumDimensionParameters& parameters);
	void init() override;
	void step(const double& t, const double& deltaT) override;
	void close() override;

	void setParameters(const SumDimensionParameters& parameters);
	SumDimensionParameters getParameters();

	~SumDimension() = default;
};