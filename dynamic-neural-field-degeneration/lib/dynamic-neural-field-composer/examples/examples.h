#pragma once

#include "simulation/simulation.h"
#include "elements/neural_field.h"
#include "elements/mexican_hat_kernel.h"
#include "elements/gauss_kernel.h"
#include "elements/normal_noise.h"
#include "elements/gauss_stimulus.h"
#include "elements/field_coupling.h"
#include "elements/sum_dimension.h"
#include "elements/degenerate_field_coupling.h"
#include "elements/degenerate_neural_field.h"



std::shared_ptr<Simulation> test_SharedTaskKnowledge();
std::shared_ptr<Simulation> test_TwoLayerField();
std::shared_ptr<Simulation> test_DegeneracyOneLayerFieldModel();

