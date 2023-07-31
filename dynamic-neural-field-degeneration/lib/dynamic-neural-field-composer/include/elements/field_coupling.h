#pragma once

#include "mathtools/mathtools.h"

#include "element.h"

#include <set>

struct FieldCouplingParameters
{
	double scalar;
	double learningRate;
};

enum class LearningRule
{
	HEBBIAN = 0,
	DELTA_WIDROW_HOFF,
	DELTA_KROGH_HERTZ
};

class FieldCoupling : public Element
{
protected:
	FieldCouplingParameters parameters;
	std::vector<std::vector<double>> weights;
	bool trained;
	LearningRule learningRule;
public:
	FieldCoupling(const std::string& id, const int& outputSize, const int& inputSize, const FieldCouplingParameters& parameters, const LearningRule& learningRule);
	
	void init() override;
	void step(const double& t, const double& deltaT) override;
	void close() override;

	void resetWeights();
	void updateWeights(const std::vector<double> input, const std::vector<double> output);
	
	const std::vector<std::vector<double>>& getWeights() const;

	~FieldCoupling() = default;

	
protected:
	void getInputFunction();
	void computeOutput();
	void scaleOutput();

	bool readWeights();
	void writeWeights();

};