#pragma once

#include "mathtools/mathtools.h"

#include "element.h"

#include <set>


struct FieldCouplingParameters
{
	double scalar;
	double learningRate;
};

class FieldCoupling : public Element
{
protected:
	FieldCouplingParameters parameters;
	std::vector<std::vector<double>> weights;
	bool trained;
public:
	FieldCoupling(const std::string& id, const uint8_t& size, const FieldCouplingParameters& parameters);
	void init() override;
	void step(const double& t, const double& deltaT) override;
	void close() override;
	void trainWeights(const std::string& inputFilename, const std::string& outputFilename, const uint16_t& iterations);
	void resetWeights();
	~FieldCoupling() = default;

	std::vector<double> readInputOrOutput(const std::string& filename, const uint8_t& line);
	void writeInputOrOutput(const std::string& filename, const std::vector<double>* data);

protected:
	void getInputFunction();
	void computeOutput();
	void scaleOutput();

	void updateWeights(const std::vector<double> input, const std::vector<double> output);
	bool readWeights();
	void writeWeights();
};