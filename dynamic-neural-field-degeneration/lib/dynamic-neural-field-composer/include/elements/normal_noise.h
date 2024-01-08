
#pragma once

#include "mathtools/mathtools.h"

#include "element.h"


namespace dnf_composer
{
	namespace element
	{
		struct NormalNoiseParameters
		{
			double amplitude;
			bool operator==(const NormalNoiseParameters& other) const
			{
				constexpr double epsilon = 1e-6; // Set an appropriate epsilon value
				return std::abs(amplitude - other.amplitude) < epsilon;
			}
		};

		class NormalNoise : public Element
		{
		private:
			NormalNoiseParameters parameters;
		public:
			NormalNoise(const ElementCommonParameters& elementCommonParameters, NormalNoiseParameters parameters);

			void init() override;
			void step(double t, double deltaT) override;
			void close() override {}

			void printParameters() override;

			void setParameters(NormalNoiseParameters parameters);
			NormalNoiseParameters getParameters() const;

			~NormalNoise() override = default;
		};
	}
}