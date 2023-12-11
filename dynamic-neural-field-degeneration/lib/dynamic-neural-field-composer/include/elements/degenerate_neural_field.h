#pragma once

#include <set>
#include "./neural_field.h"

namespace dnf_composer
{
	class DegenerateNeuralField : public element::NeuralField
	{
	private:
		element::ElementDegeneracyType degeneracyType;
		bool degenerate;
		std::vector<int> indicesForDegeneration;
		std::vector<int> degeneratedIndices;
	public:
		DegenerateNeuralField(const std::string& id, const int& size, const element::NeuralFieldParameters& parameters);

		void init() override;
		void step(double t, double deltaT) override;
		void close() override;

		void startDegeneration();
		void applyDegeneracy();

		void setDegeneracyType(element::ElementDegeneracyType degeneracyType);
		element::ElementDegeneracyType getDegeneracyType();
	private:
		void populateIndicesForDegeneration();
		void setRandomUniqueNeuronToZero();
		void calculateActivation(const double& t, const double& deltaT);
	};
}
