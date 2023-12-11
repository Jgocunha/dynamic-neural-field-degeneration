// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "elements/normal_noise.h"

namespace dnf_composer
{
	namespace element
	{
		NormalNoise::NormalNoise(const std::string& id, int size, NormalNoiseParameters parameters)
			: parameters(parameters)
		{
			if (size <= 0)
				throw Exception(ErrorCode::ELEM_INVALID_SIZE, id);

			this->label = ElementLabel::NORMAL_NOISE;
			this->uniqueName = id;
			this->size = size;
			components["output"] = std::vector<double>(size);
		}

		void NormalNoise::init()
		{
			std::ranges::fill(components["output"], 0.0);
		}

		void NormalNoise::step(double t, double deltaT)
		{
			const std::vector<double> rand = mathtools::generateNormalVector(size);

			for (int i = 0; i < size; i++)
				components["output"][i] = parameters.amplitude / sqrt(deltaT) * rand[i];
		}

		void NormalNoise::printParameters()
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
				logStream << componentName << " | ";
			}

			logStream << std::endl << "Inputs: ";
			for (const auto& inputPair : inputs)
			{
				const std::shared_ptr<Element>& inputElement = inputPair.first;
				const std::string& inputComponent = inputPair.second;

				logStream << inputElement->getUniqueName() << "->" << inputComponent << " | ";
			}

			logStream << std::endl << "NormalNoiseParameters: ";
			logStream << "Amplitude: " << parameters.amplitude << std::endl;

			user_interface::LoggerWindow::addLog(user_interface::LogLevel::_INFO, logStream.str().c_str());
		}

		void NormalNoise::setParameters(NormalNoiseParameters parameters)
		{
			this->parameters = parameters;
		}

		NormalNoiseParameters NormalNoise::getParameters() const
		{
			return parameters;
		}
	}
}