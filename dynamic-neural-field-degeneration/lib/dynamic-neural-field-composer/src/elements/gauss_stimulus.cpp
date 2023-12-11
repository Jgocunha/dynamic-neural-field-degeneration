// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "elements/gauss_stimulus.h"

namespace dnf_composer
{
	namespace element
	{
		GaussStimulus::GaussStimulus(const std::string& id, int size, const GaussStimulusParameters& parameters)
			: parameters(parameters)
		{
			if (size <= 0)
				throw Exception(ErrorCode::ELEM_INVALID_SIZE, id);

			if (parameters.position < 0 || parameters.position >= size)
				throw Exception(ErrorCode::GAUSS_STIMULUS_POSITION_OUT_OF_RANGE, id);

			this->label = ElementLabel::GAUSS_STIMULUS;
			this->uniqueName = id;
			this->size = size;
			components["output"] = std::vector<double>(size);
			this->parameters.circular = true;
			this->parameters.normalized = false;
		}

		void GaussStimulus::init()
		{
			std::vector<double> g(size);

			if (parameters.circular)
				g = mathtools::circularGauss(size, parameters.sigma, parameters.position);
			else
			{
				const std::string message = "Tried to initialize a non-circular Gaussian stimulus '" + this->getUniqueName() + "'. That is not supported yet.";
				user_interface::LoggerWindow::addLog(user_interface::LogLevel::_ERROR, message.c_str());
			}

			if (!parameters.normalized)
				for (int i = 0; i < size; i++)
					components["output"][i] = parameters.amplitude * g[i];
			else
			{
				const std::string message = "Tried to initialize a normalized Gaussian stimulus '" + this->getUniqueName() + "'. That is not supported yet.";
				user_interface::LoggerWindow::addLog(user_interface::LogLevel::_ERROR, message.c_str());
			}

			components["input"] = std::vector<double>(size);
			updateInput();
			for (int i = 0; i < size; i++)
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
			std::ostringstream logStream;

			logStream << std::left;

			logStream << "Logging element parameters" << std::endl;
			logStream << "Unique Identifier: " << uniqueIdentifier << std::endl;
			logStream << "Unique Name: " << uniqueName << std::endl;
			logStream << "Label: " << ElementLabelToString.at(label) << std::endl;
			logStream << "Size: " << size << std::endl;

			logStream << "Components: ";
			for (const auto& pair : components)
			{
				const std::string& componentName = pair.first;
				const std::vector<double>& componentValues = pair.second;

				logStream << componentName << " | ";
			}

			logStream << std::endl << "Inputs: ";
			for (const auto& inputPair : inputs)
			{
				const std::shared_ptr<Element>& inputElement = inputPair.first;
				const std::string& inputComponent = inputPair.second;

				logStream << inputElement->getUniqueName() << "->" << inputComponent << " | ";
			}

			logStream << std::endl << "GaussStimulusParameters: ";
			logStream << "Amplitude: " << parameters.amplitude << " | ";
			logStream << "Sigma: " << parameters.sigma << " | ";
			logStream << "Position: " << parameters.position << " | ";
			logStream << "Circular: " << parameters.circular << " | ";
			logStream << "Normalized: " << parameters.normalized << std::endl;

			user_interface::LoggerWindow::addLog(user_interface::LogLevel::_INFO, logStream.str().c_str());
		}

		void GaussStimulus::setParameters(const GaussStimulusParameters& parameters)
		{
			this->parameters = parameters;
			init();
		}

		GaussStimulusParameters GaussStimulus::getParameters() const
		{
			return parameters;
		}
	}
}
