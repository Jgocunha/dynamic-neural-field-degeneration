#pragma once

#include "element.h"

#include "element.h"
#include "gauss_kernel.h"
#include "gauss_stimulus.h"
#include "mexican_hat_kernel.h"
#include "neural_field.h" 
#include "normal_noise.h"
#include "gauss_field_coupling.h"
#include "field_coupling.h"

namespace dnf_composer
{
    namespace element
    {
	    struct CompoundElementParameters
	    {
	        NeuralFieldParameters nfp;
	        GaussStimulusParameters gsp;
	        GaussKernelParameters gkp;
	        MexicanHatKernelParameters mhkp;
	        NormalNoiseParameters nnp;
	        GaussFieldCouplingParameters gfcp;
			FieldCouplingParameters fcp;

			CompoundElementParameters() = default;
	    };

	    class ElementFactory
	    {
	    private:
	        std::unordered_map<ElementLabel, std::function<std::shared_ptr<Element>(const ElementCommonParameters&, const CompoundElementParameters&)>> elementCreators;

	    public:
			ElementFactory();
			std::shared_ptr<Element> create(ElementLabel type, const ElementCommonParameters& elementCommonParameters, const CompoundElementParameters& elementSpecificParameters);
	    };
    }
    
}
