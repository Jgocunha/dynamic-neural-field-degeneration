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
			elementCreators[ElementLabel::NEURAL_FIELD] = [](const std::string& id, int size, const ElementParameters& params)
				{
					return std::make_shared<NeuralField>(id, size, params.nfp);
				};

			elementCreators[ElementLabel::GAUSS_STIMULUS] = [](const std::string& id, int size, const ElementParameters& params)
				{
					return std::make_shared<GaussStimulus>(id, size, params.gsp);
				};

			elementCreators[ElementLabel::GAUSS_KERNEL] = [](const std::string& id, int size, const ElementParameters& params)
				{
					return std::make_shared<GaussKernel>(id, size, params.gkp);
				};

			elementCreators[ElementLabel::MEXICAN_HAT_KERNEL] = [](const std::string& id, int size, const ElementParameters& params)
				{
					return std::make_shared<MexicanHatKernel>(id, size, params.mhkp);
				};

			elementCreators[ElementLabel::NORMAL_NOISE] = [](const std::string& id, int size, const ElementParameters& params)
				{
					return std::make_shared<NormalNoise>(id, size, params.nnp);
				};

			elementCreators[ElementLabel::GAUSS_FIELD_COUPLING] = [](const std::string& id, int size, const ElementParameters& params)
				{
					return std::make_shared<GaussFieldCoupling>(id, size, params.gfcp);
				};

			elementCreators[ElementLabel::FIELD_COUPLING] = [](const std::string& id, int size, const ElementParameters& params)
			{
				return std::make_shared<FieldCoupling>(id, size, params.fcp);
			};
		}

		std::shared_ptr<Element> ElementFactory::create(ElementLabel type, const std::string& id, int size, const ElementParameters& params)
		{
			const auto creator = elementCreators.find(type);
			if (creator != elementCreators.end())
			{
				return creator->second(id, size, params);
			}
			// Handle unsupported element types or return a default element
			return nullptr;
		}

	}
}
