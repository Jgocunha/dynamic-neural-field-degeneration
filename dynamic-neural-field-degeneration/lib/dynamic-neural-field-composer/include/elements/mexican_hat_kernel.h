#pragma once

#include "mathtools/mathtools.h"
#include <vector>
#include <string>
#include <array>

#include "kernel.h"

namespace dnf_composer
{
	namespace element
	{
		struct MexicanHatKernelParameters
		{
			double sigmaExc;
			double amplitudeExc;
			double sigmaInh;
			double amplitudeInh;
			double amplitudeGlobal = 0.0;
			double fullSum = 0.0;
			int cutOfFactor = 5;

			bool operator==(const MexicanHatKernelParameters& other) const
			{
				constexpr double epsilon = 1e-6; 

				return std::abs(sigmaExc - other.sigmaExc) < epsilon &&
					std::abs(amplitudeExc - other.amplitudeExc) < epsilon &&
					std::abs(sigmaInh - other.sigmaInh) < epsilon &&
					std::abs(amplitudeInh - other.amplitudeInh) < epsilon &&
					std::abs(amplitudeGlobal - other.amplitudeGlobal) < epsilon &&
					std::abs(fullSum - other.fullSum) < epsilon &&
					cutOfFactor == other.cutOfFactor;
			}
		};

		class MexicanHatKernel : public Kernel
		{
		private:
			MexicanHatKernelParameters parameters;
		public:
			MexicanHatKernel(const std::string& id, int size,
				const MexicanHatKernelParameters& parameters);

			void init() override;
			void step(double t, double deltaT) override;
			void close() override;

			void printParameters() override;

			void setParameters(const MexicanHatKernelParameters& parameters);
			MexicanHatKernelParameters getParameters() const;

			~MexicanHatKernel() override = default;
		};
	}
}
