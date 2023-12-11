// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "elements/neural_field.h"

namespace dnf_composer
{
	namespace element
	{

		NeuralField::NeuralField(const std::string& id, int size,
			const NeuralFieldParameters& parameters)
			: parameters(parameters)
		{
			if (size <= 0) 
				throw Exception(ErrorCode::ELEM_INVALID_SIZE, id);

			this->label = ElementLabel::NEURAL_FIELD;
			this->uniqueName = id;
			this->size = size;

			centroid = -1.0;

			components["input"] = std::vector<double>(size);
			components["activation"] = std::vector<double>(size);
			components["output"] = std::vector<double>(size);
			components["resting level"] = std::vector<double>(size);
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

		void NeuralField::setParameters(const NeuralFieldParameters& parameters)
		{
			this->parameters = parameters;
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

			logStream << std::endl << "NeuralFieldParameters: ";
			logStream << "StartingRestingLevel: " << parameters.startingRestingLevel << " | ";
			logStream << "Tau: " << parameters.tau << " | ";

			logStream << "ActivationFunctionParameters: ";
			//logStream << "Type: " << ActivationFunctionTypeToString.at(parameters.activationFunctionParameters.type) << " | ";
			logStream << "Steepness: " << parameters.activationFunctionParameters.steepness << " | ";
			logStream << "XShift: " << parameters.activationFunctionParameters.xShift << " | ";

			logStream << std::endl << "Current centroid value: " << getCentroid() << std::endl;

			user_interface::LoggerWindow::addLog(user_interface::LogLevel::_INFO, logStream.str().c_str());
		}

		void NeuralField::calculateActivation(double t, double deltaT)
		{
			for (int i = 0; i < size; i++)
			{
				components["activation"][i] = components["activation"][i] + deltaT / parameters.tau *
					(-components["activation"][i] + components["resting level"][i] + components["input"][i]);
			}
		}

		void NeuralField::calculateOutput()
		{
			switch (parameters.activationFunctionParameters.type)
			{
			case ActivationFunctionType::Sigmoid:
				components["output"] = mathtools::sigmoid(components["activation"],
					parameters.activationFunctionParameters.steepness, static_cast<double>(parameters.activationFunctionParameters.xShift));
				break;
			case ActivationFunctionType::Heaviside:
				components["output"] = mathtools::heaviside(components["activation"], parameters.activationFunctionParameters.xShift);
				break;
			}
		}

		void NeuralField::calculateCentroid()
		{
			const std::vector<double> f_output = mathtools::heaviside(components["activation"], 0.1);

			if (*std::ranges::max_element(f_output) > 0)
			{
				const bool isAtLimits = (f_output[0] > 0) || (f_output[size - 1] > 0);

				double sumActivation = 0.0;
				double sumWeightedPositions = 0.0;

				for (int i = 0; i < size; i++)
				{
					const double activation = f_output[i];

					sumActivation += activation;

					// Calculate the circular distance from the midpoint to the position
					double distance = 0.0;
					if (isAtLimits)
						distance = fmod(static_cast<double>(i) - static_cast<double>(size) * 0.5 + static_cast<double>(size) * 10, static_cast<double>(size));
					else
						distance = fmod(static_cast<double>(i) - static_cast<double>(size) * 0.5, static_cast<double>(size));
					sumWeightedPositions += distance * activation;
				}

				static constexpr double epsilon = 1e-6;
				if (std::fabs(sumActivation) > epsilon)
				{
					// Shift the centroid back to the circular field
					centroid = fmod(static_cast<double>(size) * 0.5 + sumWeightedPositions / sumActivation, static_cast<double>(size));
					if (isAtLimits)
						centroid = (centroid >= 0 ? centroid : centroid + static_cast<double>(size));
				}
			}
			else
				centroid = -1.0;
		}
	}
}