// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#include "elements/element_parameters.h"

namespace dnf_composer
{
	namespace element
	{
		ElementSpatialDimensionParameters::ElementSpatialDimensionParameters(int x_max, double d_x)
			: x_max(x_max), size(static_cast<int>(std::round(x_max / d_x))), d_x(d_x)
		{}

		void ElementSpatialDimensionParameters::print() const
		{
			std::ostringstream logStream;
			logStream << std::left;
			logStream << "Maximum spatial dimension size: " << x_max << std::endl;
			logStream << "Spatial dimension step size: " << d_x << std::endl;
			logStream << "Number of samples in spatial dimension: " << size << std::endl;
			log(LogLevel::INFO, logStream.str());
		}

		ElementIdentifiers::ElementIdentifiers()
			: uniqueIdentifier(uniqueIdentifierCounter++), label(ElementLabel::UNINITIALIZED)
		{}

		ElementIdentifiers::ElementIdentifiers(std::string elementName)
			: uniqueIdentifier(uniqueIdentifierCounter++), uniqueName(std::move(elementName)), label(ElementLabel::UNINITIALIZED)
		{}

		void ElementIdentifiers::print() const
		{
			std::ostringstream logStream;
			logStream << std::left;
			logStream << "Unique identifier: " << uniqueIdentifier << std::endl;
			logStream << "Unique name: " << uniqueName << std::endl;
			logStream << "Label: " << ElementLabelToString.at(label) << std::endl;
			log(LogLevel::INFO, logStream.str());
		}

		ElementCommonParameters::ElementCommonParameters()
			: identifiers(), dimensionParameters()
		{}

		ElementCommonParameters::ElementCommonParameters(const std::string& elementName)
			: identifiers(elementName), dimensionParameters()
		{}

		ElementCommonParameters::ElementCommonParameters(const std::string& elementName, int x_max)
			: identifiers(elementName), dimensionParameters(x_max)
		{}

		ElementCommonParameters::ElementCommonParameters(const std::string& elementName, const ElementSpatialDimensionParameters& dimensionParameters)
			: identifiers(elementName), dimensionParameters(dimensionParameters)
		{}

		ElementCommonParameters::ElementCommonParameters(ElementIdentifiers identifiers, const ElementSpatialDimensionParameters& dimensionParameters)
			: identifiers(std::move(identifiers)), dimensionParameters(dimensionParameters)
		{}

		void ElementCommonParameters::print() const
		{
			std::ostringstream logStream;
			logStream << std::left;
			logStream << "Element common parameters:" << std::endl;
			identifiers.print();
			dimensionParameters.print();
			log(LogLevel::INFO, logStream.str());
		}
	}
}