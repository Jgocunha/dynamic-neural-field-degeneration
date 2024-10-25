#pragma once

#include <set>
#include <elements/neural_field.h>

#include "degeneration_parameters.h"


class DegenerateNeuralField : public dnf_composer::element::NeuralField
{
private:
	experiment::degeneration::ElementDegeneracyType degeneracyType;
	bool degenerate;
	std::vector<int> indicesForDegeneration;
	std::vector<int> degeneratedIndices;
	int numNeuronsToDegenerate = 1;
public:
	DegenerateNeuralField(const dnf_composer::element::ElementCommonParameters& elementCommonParameters,
		const dnf_composer::element::NeuralFieldParameters& parameters);

	void init() override;
	void step(double t, double deltaT) override;
	void reset();

	void startDegeneration();
	void applyDegeneracy();

	void setDegeneracyType(experiment::degeneration::ElementDegeneracyType degeneracyType);
	void setNumNeuronsToDegenerate(const int& numNeuronsToDegenerate);
	experiment::degeneration::ElementDegeneracyType getDegeneracyType() const;
	double getCentroid();
	void populateIndicesForDegeneration();
	void clearDegeneration();
private:
	void setRandomUniqueNeuronToZero();
	void calculateActivation(const double& t, const double& deltaT);
};
