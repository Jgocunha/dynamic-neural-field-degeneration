#pragma once


#include <algorithm>
#include <elements/field_coupling.h>

#include "degeneration_parameters.h"

class DegenerateFieldCoupling : public dnf_composer::element::FieldCoupling
{
private:
	experiment::degeneration::ElementDegeneracyType degeneracyType;
	bool degenerate;
	std::set<std::pair<int, int>> indicesForDegeneration;
	double minWeightValue = 0;
	double maxWeightValue = 0;
	double weightReductionFactor = 0.005;
	int numWeightsToDegenerate = 100;
public:
	DegenerateFieldCoupling(const dnf_composer::element::ElementCommonParameters& elementCommonParameters,
		const dnf_composer::element::FieldCouplingParameters& fc_parameters);

	void init() override;
	void step(double t, double deltaT) override;

	void startDegeneration();
	void applyDegeneracy();

	void setWeightReductionFactor(const double& factor);
	int getNumIndicesForDegeneration() const;
	void setDegeneracyType(experiment::degeneration::ElementDegeneracyType degeneracyType);
	void setNumWeightsToDegenerate(int count);
	experiment::degeneration::ElementDegeneracyType getDegeneracyType() const;
	virtual void updateWeights(const std::vector<double>& input, const std::vector<double>& output);
	void populateIndicesForDegeneration();
private:
	void setRandomWeightToRandomValue();
	void setRandomWeightToReduceValue();
	void setRandomUniqueWeightToZero();
	void findMinMaxWeightValues();
	double getWeightReductionFactor() const;
	void setRandomUniqueWeightToReduceValue();
	void setRandomUniqueWeightToRandomValue();

	std::vector<std::vector<double>> learningRuleDegenerate(std::vector<std::vector<double>>& weights,
		const std::vector<double>& input, const std::vector<double>& targetOutput, const double& learningRate) const;
};