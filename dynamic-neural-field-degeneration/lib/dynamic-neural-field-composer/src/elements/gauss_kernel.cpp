// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "elements/gauss_kernel.h"

namespace dnf_composer
{
	namespace element
	{
		GaussKernel::GaussKernel(const std::string& id, int size,
			const GaussKernelParameters& parameters)
			: parameters(parameters)
		{
			if (size <= 0)
				throw Exception(ErrorCode::ELEM_INVALID_SIZE, id);

			this->label = ElementLabel::GAUSS_KERNEL;
			this->uniqueName = id;
			this->size = size;

			components["kernel"] = std::vector<double>(size);
			components["output"] = std::vector<double>(size);
			components["input"] = std::vector<double>(size);
		}

		void GaussKernel::init()
		{
			kernelRange = mathtools::computeKernelRange(parameters.sigma, parameters.cutOfFactor, size, circular);

			if (circular)
				extIndex = mathtools::createExtendedIndex(size, kernelRange);
			else
			{
				const std::string message = "Tried to initialize a non-circular Mexican hat kernel '" + this->getUniqueName() + "'. That is not supported yet.";
				user_interface::LoggerWindow::addLog(user_interface::LogLevel::_ERROR, message.c_str());
			}

			int rangeXsize = kernelRange[0] + kernelRange[1] + 1;
			std::vector<int> rangeX(rangeXsize);
			const int startingValue = static_cast<int>(kernelRange[0]);
			std::iota(rangeX.begin(), rangeX.end(), -startingValue);
			std::vector<double> gauss(size);
			if (!normalized)
				gauss = mathtools::gaussNorm(rangeX, 0.0, parameters.sigma);
			else
			{
				const std::string message = "Tried to initialize a normalized Mexican hat kernel '" + this->getUniqueName() + "'. That is not supported yet.";
				user_interface::LoggerWindow::addLog(user_interface::LogLevel::_ERROR, message.c_str());
			}

			components["kernel"].resize(rangeX.size());
			for (int i = 0; i < components["kernel"].size(); i++)
				components["kernel"][i] = parameters.amplitude * gauss[i];

			components["input"].resize(extIndex.size());
			parameters.fullSum = 0;
			std::ranges::fill(components["input"], 0.0);
		}

		void GaussKernel::step(double t, double deltaT)
		{

			updateInput();

			parameters.fullSum = std::accumulate(components["input"].begin(), components["input"].end(), (double)0.0);

			std::vector<double> convolution(size);
			const std::vector<double> subDataInput = mathtools::obtainCircularVector(extIndex, components["input"]);


			if (circular)
				convolution = mathtools::conv_valid(subDataInput, components["kernel"]);
			else
				convolution = mathtools::conv(subDataInput, components["kernel"]);

			for (int i = 0; i < components["output"].size(); i++)
				components["output"][i] = convolution[i];
			for (int i = 0; i < components["output"].size(); i++)
				components["output"][i] += parameters.amplitudeGlobal;

		}

		void GaussKernel::close()
		{
		}

		void GaussKernel::printParameters()
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

			logStream << std::endl << "GaussKernelParameters: ";
			logStream << "Amplitude: " << parameters.amplitude << " | ";
			logStream << "Amplitude Global: " << parameters.amplitudeGlobal << " | ";
			logStream << "Sigma: " << parameters.sigma << " | ";
			logStream << "Cut-Off Factor: " << parameters.cutOfFactor << " | ";
			logStream << "Circular: " << circular << " | ";
			logStream << "Normalized: " << normalized << std::endl;

			user_interface::LoggerWindow::addLog(user_interface::LogLevel::_INFO, logStream.str().c_str());
		}

		void GaussKernel::setParameters(const GaussKernelParameters& parameters)
		{
			this->parameters = parameters;
			init();
		}

		GaussKernelParameters GaussKernel::getParameters() const
		{
			return parameters;
		}
	}
}
