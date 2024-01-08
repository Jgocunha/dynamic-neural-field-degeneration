#pragma once


#include <algorithm>
#include "./field_coupling.h"

namespace dnf_composer
{
	namespace element
	{
		class DegenerateFieldCoupling : public element::FieldCoupling
		{
		private:
			element::ElementDegeneracyType degeneracyType;
			bool degenerate;
			std::set<std::pair<int, int>> indicesForDegeneration;
			double minWeightValue = 0;
			double maxWeightValue = 0;
			double weightReductionFactor = 0.2;
			int numWeightsToDegenerate = 100;
		public:
			DegenerateFieldCoupling(const ElementCommonParameters& elementCommonParameters, const FieldCouplingParameters& parameters);

			void init() override;
			void step(double t, double deltaT) override;

			void startDegeneration();
			void applyDegeneracy();

			void setWeightReductionFactor(const double& factor);
			void setDegeneracyType(ElementDegeneracyType degeneracyType);
			void setNumWeightsToDegenerate(const int& numWeightsToDegenerate);
			ElementDegeneracyType getDegeneracyType() const;
			void updateWeights(const std::vector<double> input, const std::vector<double> output);
		private:
			void populateIndicesForDegeneration();
			void setRandomWeightToRandomValue();
			void setRandomWeightToReduceValue();
			void setRandomUniqueWeightToZero();
			void findMinMaxWeightValues();
			double getWeightReductionFactor();
			void setRandomUniqueWeightToReduceValue();
			void setRandomUniqueWeightToRandomValue();

			std::vector<std::vector<double>> learningRuleDegenerate(std::vector<std::vector<double>>& weights,
				const std::vector<double>& input, const std::vector<double>& targetOutput, const double& learningRate) const;
		};
	}
}
