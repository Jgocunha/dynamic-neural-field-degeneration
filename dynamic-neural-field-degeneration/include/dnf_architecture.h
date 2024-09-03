#pragma once

#include "simulation/simulation.h"
//#include "elements/degenerate_neural_field.h"
//#include "elements/degenerate_field_coupling.h"
#include "elements/gauss_kernel.h"
#include "elements/mexican_hat_kernel.h"
#include "elements/gauss_stimulus.h"
#include "elements/normal_noise.h"

std::shared_ptr<dnf_composer::Simulation> getExperimentSimulation();