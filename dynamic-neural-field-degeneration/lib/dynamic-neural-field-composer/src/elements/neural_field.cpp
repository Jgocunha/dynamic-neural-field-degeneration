// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "elements/neural_field.h"

namespace dnf_composer
{
	namespace element
	{
		NeuralField::NeuralField(const ElementCommonParameters& elementCommonParameters, const NeuralFieldParameters& parameters)
			: Element(elementCommonParameters), parameters(parameters), centroid(-1)
		{
			commonParameters.identifiers.label = ElementLabel::NEURAL_FIELD;
			components["activation"] = std::vector<double>(commonParameters.dimensionParameters.size);
			components["resting level"] = std::vector<double>(commonParameters.dimensionParameters.size);
		}

		void NeuralField::init()
		{
			std::ranges::fill(components["activation"], parameters.startingRestingLevel);
			std::ranges::fill(components["input"], 0.0);
			std::ranges::fill(components["resting level"], parameters.startingRestingLevel);

			calculateOutput();
		}

		void NeuralField::step(double t, double deltaT)
		{
			updateInput();
			calculateActivation(t, deltaT);
			calculateOutput();
			calculateCentroid();
		}

		void NeuralField::close()
		{
		}

		void NeuralField::setParameters(const NeuralFieldParameters& neuralFieldParameters)
		{
			parameters = neuralFieldParameters;
			init();
		}

		NeuralFieldParameters NeuralField::getParameters() const
		{
			return parameters;
		}

		double NeuralField::getCentroid() const
		{
			return centroid;
		}

		void NeuralField::printParameters()
		{
			printCommonParameters();

			std::ostringstream logStream;

			logStream << "Logging specific element parameters" << std::endl;
			logStream << "StartingRestingLevel: " << parameters.startingRestingLevel << std::endl;
			logStream << "Tau: " << parameters.tau << std::endl;

			//logStream << "ActivationFunctionParameters: ";
			////logStream << "Type: " << ActivationFunctionTypeToString.at(parameters.activationFunctionParameters.type) << " | ";
			//logStream << "Steepness: " << parameters.activationFunction. << " | ";
			//logStream << "XShift: " << parameters.activationFunctionParameters.xShift << " | ";

			logStream << std::endl << "Current centroid value: " << getCentroid() << std::endl;

			log(LogLevel::INFO, logStream.str());
		}

		void NeuralField::calculateActivation(double t, double deltaT)
		{
			for (int i = 0; i < commonParameters.dimensionParameters.size; i++)
			{
				components["activation"][i] = components["activation"][i] + deltaT / parameters.tau *
					(-components["activation"][i] + components["resting level"][i] + components["input"][i]);
			}
		}

		void NeuralField::calculateOutput()
		{

			components["output"] = parameters.activationFunction->operator()(components["activation"]);
		}

		void NeuralField::calculateCentroid()
		{
			const std::vector<double> f_output = mathtools::heaviside(components["activation"], 0.1);

			if (*std::ranges::max_element(f_output) > 0)
			{
				const bool isAtLimits = (f_output[0] > 0) || (f_output[commonParameters.dimensionParameters.size - 1] > 0);

				double sumActivation = 0.0;
				double sumWeightedPositions = 0.0;

				for (int i = 0; i < commonParameters.dimensionParameters.size; i++)
				{
					const double activation = f_output[i];

					sumActivation += activation;

					// Calculate the circular distance from the midpoint to the position
					double distance = 0.0;
					if (isAtLimits)
						distance = fmod(static_cast<double>(i) - static_cast<double>(commonParameters.dimensionParameters.size) * 0.5 
							+ static_cast<double>(commonParameters.dimensionParameters.size) * 10, static_cast<double>(commonParameters.dimensionParameters.size));
					else
						distance = fmod(static_cast<double>(i) - static_cast<double>(commonParameters.dimensionParameters.size) * 0.5, static_cast<double>(commonParameters.dimensionParameters.size));
					sumWeightedPositions += distance * activation;
				}

				static constexpr double epsilon = 1e-6;
				if (std::fabs(sumActivation) > epsilon)
				{
					// Shift the centroid back to the circular field
					centroid = fmod(static_cast<double>(commonParameters.dimensionParameters.size) * 0.5 + sumWeightedPositions / sumActivation, static_cast<double>(commonParameters.dimensionParameters.size));
					if (isAtLimits)
						centroid = (centroid >= 0 ? centroid : centroid + static_cast<double>(commonParameters.dimensionParameters.size));
				}
			}
			else
				centroid = -1.0;
		}
	}
}