#pragma once

#include "element.h"
#include "mathtools/mathtools.h"


namespace dnf_composer
{
	struct WeightedCoupling
	{
		double x_i, x_j, w_i_j;
	};

	namespace element
	{
		struct GaussFieldCouplingParameters
		{
			int inputFieldSize = 0;
			double sigma = 0.0;
			std::vector<WeightedCoupling> couplings;

			void addCoupling(const WeightedCoupling& coupling)
			{
				couplings.emplace_back(coupling);
			}
		};

		class GaussFieldCoupling : public Element
		{
		private:
			GaussFieldCouplingParameters gfcp;
		public:
			GaussFieldCoupling(const std::string& id, int size, const GaussFieldCouplingParameters& gfcp);

			void addCoupling(const WeightedCoupling& coupling);

			void init() override;
			void step(double t, double deltaT) override;
			void close() override;
			void printParameters() override;

			GaussFieldCouplingParameters getParameters() const;

			~GaussFieldCoupling() override = default;
		private:
			void updateOutput();
		};

	}
}




