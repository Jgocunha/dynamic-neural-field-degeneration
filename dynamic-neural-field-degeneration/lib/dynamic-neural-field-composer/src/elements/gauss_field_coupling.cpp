// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "elements/gauss_field_coupling.h"

namespace dnf_composer
{
	namespace element
	{
		GaussFieldCoupling::GaussFieldCoupling(const std::string& id, int size, const GaussFieldCouplingParameters& gfcp)
			: gfcp(gfcp)
		{
			if (size <= 0)
				throw Exception(ErrorCode::ELEM_INVALID_SIZE, id);
			if (gfcp.inputFieldSize <= 0)
				throw Exception(ErrorCode::ELEM_SIZE_NOT_ALLOWED, id);
			if (gfcp.sigma <= 0)
				throw Exception(ErrorCode::ELEM_INVALID_PARAMETER, id);

			this->label = ElementLabel::GAUSS_FIELD_COUPLING;
			this->uniqueName = id;
			this->size = size;

			components["input"] = std::vector<double>(gfcp.inputFieldSize);
			components["output"] = std::vector<double>(size);
		}

		void GaussFieldCoupling::init()
		{
			std::ranges::fill(components["output"], 0.0);
		}

		void GaussFieldCoupling::step(double t, double deltaT)
		{
			updateInput();
			updateOutput();
		}

		void GaussFieldCoupling::close()
		{
		}

		void GaussFieldCoupling::printParameters()
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

			logStream << std::endl << "GaussFieldCouplingParameters: ";
			logStream << "Input Field Size: " << gfcp.inputFieldSize << " | ";
			logStream << "Sigma: " << gfcp.sigma << " | ";

			logStream << "Couplings: ";
			for (const auto& coupling : gfcp.couplings)
			{
				logStream << "x_i: " << coupling.x_i << ", x_j: " << coupling.x_j << ", w_i_j: " << coupling.w_i_j << " | ";
			}

			user_interface::LoggerWindow::addLog(user_interface::LogLevel::_INFO, logStream.str().c_str());

		}

		void GaussFieldCoupling::updateOutput()
		{
			std::vector<double> summedGaussians(size);
			std::ranges::fill(summedGaussians, 0.0);

			for (const auto& coupling : gfcp.couplings)
			{
				const auto activationAtx_i = components["input"][static_cast<int>(coupling.x_i)];
				if (activationAtx_i > 0.0)
				{
					std::vector<double> gauss = mathtools::circularGauss(size, gfcp.sigma, coupling.x_j);
					for (auto& element : gauss)
						element *= coupling.w_i_j * activationAtx_i * element;

					for (int i = 0; i < size; i++)
						summedGaussians[i] += gauss[i];
				}
			}

			components["output"] = std::move(summedGaussians);
		}

		void GaussFieldCoupling::addCoupling(const WeightedCoupling& coupling)
		{
			gfcp.couplings.emplace_back(coupling);
		}

		GaussFieldCouplingParameters GaussFieldCoupling::getParameters() const
		{
			return gfcp;
		}
	}
}