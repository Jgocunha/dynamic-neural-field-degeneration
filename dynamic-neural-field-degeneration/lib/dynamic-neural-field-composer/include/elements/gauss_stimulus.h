#pragma once

#include <iostream>

#include "mathtools/mathtools.h"
#include "element.h"


namespace dnf_composer
{
	namespace element
	{
		struct GaussStimulusParameters
		{
			double sigma = 5.0;
			double amplitude = 15.0;
			double position = 0.0;
			bool circular = true;
			bool normalized = false;

			bool operator==(const GaussStimulusParameters& other) const
			{
				constexpr double epsilon = 1e-6;

				return std::abs(sigma - other.sigma) < epsilon &&
					std::abs(position - other.position) < epsilon &&
					std::abs(amplitude - other.amplitude) < epsilon &&
					circular == other.circular &&
					normalized == other.normalized;
			}
		};

		class GaussStimulus : public Element
		{
		private:
			GaussStimulusParameters parameters;
		public:
			GaussStimulus(const std::string& id, int size,
				const GaussStimulusParameters& parameters);

			void init() override;
			void step(double t, double deltaT) override;
			void close() override;

			void printParameters() override;

			void setParameters(const GaussStimulusParameters& parameters);
			GaussStimulusParameters getParameters() const;
			~GaussStimulus() override = default;
		};
	}
}
