#pragma once

#include <set>
#include <elements/neural_field.h>

#include "degenerate_element_parameters.h"


class DegenerateNeuralField : public dnf_composer::element::NeuralField
{
private:
	ElementDegeneracyType degeneracyType;
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

	void setDegeneracyType(ElementDegeneracyType degeneracyType);
	void setNumNeuronsToDegenerate(const int& numNeuronsToDegenerate);
	ElementDegeneracyType getDegeneracyType();
	double getCentroid();
private:
	void populateIndicesForDegeneration();
	void setRandomUniqueNeuronToZero();
	void calculateActivation(const double& t, const double& deltaT);
};
