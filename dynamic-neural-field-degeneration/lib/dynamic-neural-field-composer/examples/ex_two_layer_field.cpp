
#include "examples.h"


std::shared_ptr<Simulation> test_TwoLayerField()
{
    // shared parameters
    uint32_t fieldSize = 100;
    double sigmaExc = 5;
    double sigmaInh = 10;

    // create simulator object
    std::shared_ptr<Simulation> sim = std::make_shared<Simulation>();

    // create neural fields
    ActivationFunctionParameters afp = { ActivationFunctionType::Sigmoid, 4, 0 };
    std::shared_ptr<NeuralField> neural_field_u(new NeuralField("field u", fieldSize, { 20, -5}, afp));
    std::shared_ptr<NeuralField> neural_field_v(new NeuralField("field v", fieldSize, { 5, -5}, afp));

    sim->addElement(neural_field_u);
    sim->addElement(neural_field_v);

    // create inputs and add them to the simulation
    std::shared_ptr<GaussStimulus> gauss_stim_a(new GaussStimulus("stim a", fieldSize, { sigmaExc, 15, 25 }));
    std::shared_ptr<GaussStimulus> gauss_stim_b(new GaussStimulus("stim b", fieldSize, { sigmaExc, 15, 50 }));
    std::shared_ptr<GaussStimulus> gauss_stim_c(new GaussStimulus("stim c", fieldSize, { sigmaExc, 15, 75 }));
    std::shared_ptr<GaussStimulus> stimulus_sum(new GaussStimulus("stim sum", fieldSize, { 0, 0, 0 }));

    sim->addElement(gauss_stim_a);
    sim->addElement(gauss_stim_b);
    sim->addElement(gauss_stim_c);
    sim->addElement(stimulus_sum);

    // create interactions and add them to the simulation
    std::shared_ptr<GaussKernel> gaussKernel_u_u(new GaussKernel("u - u", fieldSize, { sigmaExc, 1 }));
    std::shared_ptr<GaussKernel> gaussKernel_u_v(new GaussKernel("u - v", fieldSize, {sigmaExc, 10}));
    std::shared_ptr<GaussKernel> gaussKernel_v_u(new GaussKernel("v - u (local)", fieldSize, { sigmaInh, 1 }));
    std::shared_ptr<SumDimension> sum_dimension_v_u(new SumDimension("v - u (global)", fieldSize, {2, 1, -1} ));

    sim->addElement(gaussKernel_u_u);
    sim->addElement(gaussKernel_u_v);
    sim->addElement(gaussKernel_v_u);
    sim->addElement(sum_dimension_v_u);

    // create noise stimulus and noise kernel
    std::shared_ptr<NormalNoise> noise_u(new NormalNoise("noise u", fieldSize, { 1 }));
    std::shared_ptr<NormalNoise> noise_v(new NormalNoise("noise v", fieldSize, { 1 }));
    std::shared_ptr<GaussKernel> noise_kernel_u(new GaussKernel("noise kernel u", fieldSize, { 0.25, 0.2 }));
    std::shared_ptr<GaussKernel> noise_kernel_v(new GaussKernel("noise kernel v", fieldSize, { 0.25, 0.2 }));

    sim->addElement(noise_u);
    sim->addElement(noise_v);
    sim->addElement(noise_kernel_u);
    sim->addElement(noise_kernel_v);

    // define the interactions between the elements
    stimulus_sum->addInput(gauss_stim_a);
    stimulus_sum->addInput(gauss_stim_b);
    stimulus_sum->addInput(gauss_stim_c);

    neural_field_u->addInput(stimulus_sum);
    neural_field_u->addInput(gaussKernel_u_u);
    neural_field_u->addInput(gaussKernel_v_u);
    neural_field_u->addInput(sum_dimension_v_u);
    neural_field_u->addInput(noise_kernel_u);

    neural_field_v->addInput(gaussKernel_u_v);
    neural_field_v->addInput(noise_kernel_v);

    gaussKernel_u_u->addInput(neural_field_u);
    gaussKernel_u_v->addInput(neural_field_u);
    gaussKernel_v_u->addInput(neural_field_v);

    sum_dimension_v_u->addInput(neural_field_v);

    noise_kernel_u->addInput(noise_u);
    noise_kernel_v->addInput(noise_v);

    return sim;
}