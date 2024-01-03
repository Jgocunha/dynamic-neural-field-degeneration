#pragma once

#include <set>
#include "./neural_field.h"

namespace dnf_composer
{
	namespace element
	{
		class DegenerateNeuralField : public NeuralField
		{
		private:
			ElementDegeneracyType degeneracyType;
			bool degenerate;
			std::vector<int> indicesForDegeneration;
			std::vector<int> degeneratedIndices;
		public:
			DegenerateNeuralField(const ElementCommonParameters& elementCommonParameters, const NeuralFieldParameters& parameters);

			void init() override;
			void step(double t, double deltaT) override;
			void close() override;

			void startDegeneration();
			void applyDegeneracy();

			void setDegeneracyType(ElementDegeneracyType degeneracyType);
			ElementDegeneracyType getDegeneracyType();
		private:
			void populateIndicesForDegeneration();
			void setRandomUniqueNeuronToZero();
			void calculateActivation(const double& t, const double& deltaT);
		};
	}
}
