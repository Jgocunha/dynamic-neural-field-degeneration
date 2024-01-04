// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "elements/gauss_stimulus.h"

namespace dnf_composer
{
	namespace element
	{
		GaussStimulus::GaussStimulus(const ElementCommonParameters& elementCommonParameters, const GaussStimulusParameters& parameters)
			: Element(elementCommonParameters), parameters(parameters)
		{
			if (parameters.position < 0 || parameters.position >= elementCommonParameters.dimensionParameters.x_max)
				throw Exception(ErrorCode::GAUSS_STIMULUS_POSITION_OUT_OF_RANGE, elementCommonParameters.identifiers.uniqueName);

			this->commonParameters.identifiers.label = ElementLabel::GAUSS_STIMULUS;
		}

		void GaussStimulus::init()
		{
			std::vector<double> g(commonParameters.dimensionParameters.size);

			if (parameters.circular)
				g = mathtools::circularGauss(commonParameters.dimensionParameters.size, parameters.sigma, parameters.position);
			else
			{
				const std::string message = "Tried to initialize a non-circular Gaussian stimulus '" + this->getUniqueName() + "'. That is not supported yet. \n";
				log(LogLevel::ERROR_, message);
			}

			if (!parameters.normalized)
				for (int i = 0; i < commonParameters.dimensionParameters.size; i++)
					components["output"][i] = parameters.amplitude * g[i];
			else
			{
				const std::string message = "Tried to initialize a normalized Gaussian stimulus '" + this->getUniqueName() + "'. That is not supported yet. \n";
				log(LogLevel::ERROR_, message);
			}

			components["input"] = std::vector<double>(commonParameters.dimensionParameters.size);
			updateInput();
			for (int i = 0; i < commonParameters.dimensionParameters.size; i++)
				components["output"][i] += components["input"][i];
		}

		void GaussStimulus::step(double t, double deltaT)
		{
		}

		void GaussStimulus::close()
		{
		}

		void GaussStimulus::printParameters()
		{
			printCommonParameters();

			std::ostringstream logStream;

			logStream << "Logging specific element parameters" << std::endl;
			logStream << "Amplitude: " << parameters.amplitude << std::endl;
			logStream << "Sigma: " << parameters.sigma << std::endl;
			logStream << "Position: " << parameters.position << std::endl;
			logStream << "Circular: " << parameters.circular << std::endl;
			logStream << "Normalized: " << parameters.normalized << std::endl;

			log(LogLevel::INFO, logStream.str());
		}

		void GaussStimulus::setParameters(const GaussStimulusParameters& gaussStimulusParameters)
		{
			parameters = gaussStimulusParameters;
			init();
		}

		GaussStimulusParameters GaussStimulus::getParameters() const
		{
			return parameters;
		}
	}
}
