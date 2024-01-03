// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "./elements/activation_function.h"

namespace dnf_composer
{
	namespace element
	{
		SigmoidFunction::SigmoidFunction(double x_shift, double steepness)
		: x_shift(x_shift), steepness(steepness)
		{}

		std::vector<double> SigmoidFunction::operator()(const std::vector<double>& input)
		{
			return mathtools::sigmoid(input, steepness, x_shift);
		}

		std::unique_ptr<ActivationFunction> SigmoidFunction::clone() const 
		{
			return std::make_unique<SigmoidFunction>(*this);
		}

		double SigmoidFunction::getSteepness() const
		{
			return steepness;
		}

		double SigmoidFunction::getXShift() const
		{
			return x_shift;
		}

		HeavisideFunction::HeavisideFunction(double x_shift)
		: x_shift(x_shift)
		{}

		std::vector<double> HeavisideFunction::operator()(const std::vector<double>& input)
		{
			return mathtools::heaviside(input, x_shift);
		}

		std::unique_ptr<ActivationFunction> HeavisideFunction::clone() const 
		{
			return std::make_unique<HeavisideFunction>(*this);
		}

		double HeavisideFunction::getXShift() const
		{
			return x_shift;
		}
	}
}