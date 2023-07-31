#pragma once

#include "./field_coupling.h"

class DegenerateFieldCoupling : public FieldCoupling
{
private:
	ElementDegeneracyType degeneracyType;
	bool degenerate;
	std::set<std::pair<int, int>> indicesForDegeneration;
	double minWeightValue = 0;
	double maxWeightValue = 0;
	double weightReductionFactor = 0.4;
public:
	DegenerateFieldCoupling(const std::string& id, const int& outputSize, const int& inputSize, const FieldCouplingParameters& parameters, const LearningRule& learningRule);

	void init() override;
	void step(const double& t, const double& deltaT) override;

	void startDegeneration();
	void applyDegeneracy();

	void setWeightReductionFactor(const double& factor);
	void setDegeneracyType(ElementDegeneracyType degeneracyType);
	ElementDegeneracyType getDegeneracyType();
private:
	void populateIndicesForDegeneration();
	void setRandomWeightToRandomValue();
	void setRandomWeightToReduceValue();
	void setRandomUniqueWeightToZero();
	void findMinMaxWeightValues();
	double getWeightReductionFactor();
};
