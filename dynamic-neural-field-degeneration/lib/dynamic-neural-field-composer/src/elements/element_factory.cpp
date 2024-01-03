// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "elements/element_factory.h"


namespace dnf_composer
{
	namespace element
	{
		ElementFactory::ElementFactory()
		{
			// Register element creators for each element type
			elementCreators[ElementLabel::NEURAL_FIELD] = [](const ElementCommonParameters& elementCommonParameters, const CompoundElementParameters& elementSpecificParameters)
				{
					return std::make_shared<NeuralField>(elementCommonParameters, elementSpecificParameters.nfp);
				};

			elementCreators[ElementLabel::GAUSS_STIMULUS] = [](const ElementCommonParameters& elementCommonParameters, const CompoundElementParameters& elementSpecificParameters)
				{
					return std::make_shared<GaussStimulus>(elementCommonParameters, elementSpecificParameters.gsp);
				};

			elementCreators[ElementLabel::GAUSS_KERNEL] = [](const ElementCommonParameters& elementCommonParameters, const CompoundElementParameters& elementSpecificParameters)
				{
					return std::make_shared<GaussKernel>(elementCommonParameters, elementSpecificParameters.gkp);
				};

			elementCreators[ElementLabel::MEXICAN_HAT_KERNEL] = [](const ElementCommonParameters& elementCommonParameters, const CompoundElementParameters& elementSpecificParameters)
				{
					return std::make_shared<MexicanHatKernel>(elementCommonParameters, elementSpecificParameters.mhkp);
				};

			elementCreators[ElementLabel::NORMAL_NOISE] = [](const ElementCommonParameters& elementCommonParameters, const CompoundElementParameters& elementSpecificParameters)
				{
					return std::make_shared<NormalNoise>(elementCommonParameters, elementSpecificParameters.nnp);
				};

			elementCreators[ElementLabel::GAUSS_FIELD_COUPLING] = [](const ElementCommonParameters& elementCommonParameters, const CompoundElementParameters& elementSpecificParameters)
				{
					return std::make_shared<GaussFieldCoupling>(elementCommonParameters, elementSpecificParameters.gfcp);
				};

			elementCreators[ElementLabel::FIELD_COUPLING] = [](const ElementCommonParameters& elementCommonParameters, const CompoundElementParameters& elementSpecificParameters)
			{
				return std::make_shared<FieldCoupling>(elementCommonParameters, elementSpecificParameters.fcp);
			};
		}

		std::shared_ptr<Element> ElementFactory::create(ElementLabel type, const ElementCommonParameters& elementCommonParameters, const CompoundElementParameters& elementSpecificParameters)
		{
			const auto creator = elementCreators.find(type);
			if (creator != elementCreators.end())
			{
				return creator->second(elementCommonParameters, elementSpecificParameters);
			}
			// Handle unsupported element types or return a default element
			return nullptr;
		}

	}
}
