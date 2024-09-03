#pragma once


#include <algorithm>

#include "degenerate_element_parameters.h"
#include "elements/field_coupling.h"

class DegenerateFieldCoupling : public dnf_composer::element::FieldCoupling
{
private:
	ElementDegeneracyType degeneracyType;
	bool degenerate;
	std::set<std::pair<int, int>> indicesForDegeneration;
	double minWeightValue = 0;
	double maxWeightValue = 0;
	double weightReductionFactor = 0.4;
public:
	DegenerateFieldCoupling(const dnf_composer::element::ElementCommonParameters& elementCommonParameters,
		const dnf_composer::element::FieldCouplingParameters& fc_parameters);
	void init() override;
	void step(double t, double deltaT) override;

	void startDegeneration();
	void applyDegeneracy();

	void setWeightReductionFactor(const double& factor);
	void setDegeneracyType(ElementDegeneracyType degeneracyType);
	ElementDegeneracyType getDegeneracyType();
	void updateWeights(const std::vector<double> input, const std::vector<double> output);
private:
	void populateIndicesForDegeneration();
	void setRandomWeightToRandomValue();
	void setRandomWeightToReduceValue();
	void setRandomUniqueWeightToZero();
	void findMinMaxWeightValues();
	double getWeightReductionFactor();

	std::vector<std::vector<double>> learningRuleDegenerate(std::vector<std::vector<double>>& weights,
		const std::vector<double>& input, const std::vector<double>& targetOutput, const double& learningRate);
};