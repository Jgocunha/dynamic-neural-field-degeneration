#pragma once

#include <set>
#include "./neural_field.h"

class DegenerateNeuralField : public NeuralField
{
private:
	ElementDegeneracyType degeneracyType;
	bool degenerate;
	std::vector<int> indicesForDegeneration;
	std::vector<int> degeneratedIndices;
public:
    DegenerateNeuralField(const std::string& id, const int& size, const NeuralFieldParameters& parameters,
		const ActivationFunctionParameters& activationFunctionParameters);

	void init() override;
	void step(const double& t, const double& deltaT) override;
	void close() override;

	void startDegeneration();
	void applyDegeneracy();

	void setDegeneracyType(ElementDegeneracyType degeneracyType);
	ElementDegeneracyType getDegeneracyType() const;
private:
	void populateIndicesForDegeneration();
	void setRandomUniqueNeuronToZero();
	void calculateActivation(const double& t, const double& deltaT);
};
