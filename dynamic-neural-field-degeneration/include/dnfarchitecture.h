#pragma once

#include "../lib/dynamic-neural-field-composer/include/simulation/simulation.h"
#include "../lib/dynamic-neural-field-composer/include/elements/degenerate_neural_field.h"
#include "../lib/dynamic-neural-field-composer/include/elements/degenerate_field_coupling.h"
#include "../lib/dynamic-neural-field-composer/include/elements/gauss_kernel.h"
#include "../lib/dynamic-neural-field-composer/include/elements/mexican_hat_kernel.h"
#include "../lib/dynamic-neural-field-composer/include/elements/gauss_stimulus.h"
#include "../lib/dynamic-neural-field-composer/include/elements/normal_noise.h"
#include "../lib/dynamic-neural-field-composer/include/wizards/field_coupling_wizard.h"


class DNFarchitecture
{
private:
	std::shared_ptr<Simulation> simulation;
public:
	DNFarchitecture();
	~DNFarchitecture();

	std::shared_ptr<Simulation> getSimulation();
private:
	void setup();
};