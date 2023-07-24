#pragma once

#include <iostream>

#include "mathtools/mathtools.h"
#include "element.h"

struct GaussStimulusParameters
{
	double sigma;
	double amplitude;
	double position;
	bool circular = true;
	bool normalized = false;

	// Overload the == operator
	bool operator==(const GaussStimulusParameters& other) const
	{
		return sigma == other.sigma &&
			position == other.position &&
			amplitude == other.amplitude &&
			circular == other.circular &&
			normalized == other.normalized;
	}
};

class GaussStimulus : public Element
{
private:
	GaussStimulusParameters parameters;
public:
	GaussStimulus(std::string id, const int& size,
		const GaussStimulusParameters& parameters);
	void init() override;
	void step(const double& t, const double& deltaT) override;
	void close() override;
	void setParameters(const GaussStimulusParameters& parameters);
	GaussStimulusParameters getParameters();
	GaussStimulus operator+(const GaussStimulus& other) const; // Overload the + operator
	std::shared_ptr<GaussStimulus> operator+(const std::shared_ptr<GaussStimulus>& other) const; // Overload the + operator
	~GaussStimulus();
};