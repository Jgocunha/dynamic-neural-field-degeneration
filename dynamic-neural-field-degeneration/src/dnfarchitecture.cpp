#include "../include/dnfarchitecture.h"

DNFarchitecture::DNFarchitecture()
{
	simulation = std::make_shared<Simulation>(2, 0, 0);
}

DNFarchitecture::~DNFarchitecture()
{
}

void DNFarchitecture::setup()
{
    double fieldSize = 100.0;

    // add gaussian inputs
    double offset = 1.5;
    GaussStimulusParameters gsp = { 3, 15, 20 };
    gsp.position = 12.5 + offset;
    std::shared_ptr<GaussStimulus> stimulus_a(new GaussStimulus("gauss stimulus 1", fieldSize, gsp));
    gsp.position = 25 + offset;
    std::shared_ptr<GaussStimulus> stimulus_b(new GaussStimulus("gauss stimulus 2", fieldSize, gsp));
    gsp.position = 37.5 + offset;
    std::shared_ptr<GaussStimulus> stimulus_c(new GaussStimulus("gauss stimulus 3", fieldSize, gsp));
    gsp.position = 50 + offset;
    std::shared_ptr<GaussStimulus> stimulus_d(new GaussStimulus("gauss stimulus 4", fieldSize, gsp));
    gsp.position = 62.5 + offset;
    std::shared_ptr<GaussStimulus> stimulus_e(new GaussStimulus("gauss stimulus 5", fieldSize, gsp));
    gsp.position = 75 + offset;
    std::shared_ptr<GaussStimulus> stimulus_f(new GaussStimulus("gauss stimulus 6", fieldSize, gsp));
    gsp.position = 87.5 + offset;
    std::shared_ptr<GaussStimulus> stimulus_g(new GaussStimulus("gauss stimulus 7", fieldSize, gsp));

    simulation->addElement(stimulus_a);
    simulation->addElement(stimulus_b);
    simulation->addElement(stimulus_c);
    simulation->addElement(stimulus_d);
    simulation->addElement(stimulus_e);
    simulation->addElement(stimulus_f);
    simulation->addElement(stimulus_g);

    // create neural fields
    ActivationFunctionParameters afp = { ActivationFunctionType::Heaviside, 0.0, 0.2 };
    NeuralFieldParameters nfp1 = { 20, -10 };
    NeuralFieldParameters nfp2 = { 20, -5 };
    std::shared_ptr<DegenerateNeuralField> neural_field_u(new DegenerateNeuralField("field u", fieldSize, nfp1, afp));
    std::shared_ptr<DegenerateNeuralField> neural_field_v(new DegenerateNeuralField("field v", fieldSize, nfp2, afp));

    simulation->addElement(neural_field_u);
    simulation->addElement(neural_field_v);

    // create interactions and add them to the simulationulation
    GaussKernelParameters gkp1;
    gkp1.amplitude = 20;  // self-sustained (without input)
    gkp1.sigma = 5;
    std::shared_ptr<GaussKernel> gaussKernel_u_u(new GaussKernel("u - u", fieldSize, gkp1)); // self-excitation u-v
    simulation->addElement(gaussKernel_u_u);

    GaussKernelParameters gkp2;
    gkp2.amplitude = 6;  // self-stabilized (with input)
    gkp2.sigma = 5;
    std::shared_ptr<GaussKernel> gaussKernel_v_v(new GaussKernel("v - v", fieldSize, gkp2)); // self-excitation v-v
    simulation->addElement(gaussKernel_v_v);

    std::shared_ptr<DegenerateFieldCoupling> coupling_u_v(new DegenerateFieldCoupling("u - v", fieldSize, { 0.75, 0.1 }));
    simulation->addElement(coupling_u_v);

    // create noise stimulus and noise kernel
    std::shared_ptr<NormalNoise> noise_u(new NormalNoise("noise u", fieldSize, { 1 }));
    std::shared_ptr<NormalNoise> noise_v(new NormalNoise("noise v", fieldSize, { 1 }));
    std::shared_ptr<GaussKernel> noise_kernel_u(new GaussKernel("noise kernel u", fieldSize, { 0.25, 0.2 }));
    std::shared_ptr<GaussKernel> noise_kernel_v(new GaussKernel("noise kernel v", fieldSize, { 0.25, 0.2 }));

    simulation->addElement(noise_u);
    simulation->addElement(noise_v);
    simulation->addElement(noise_kernel_u);
    simulation->addElement(noise_kernel_v);

    // define the interactions between the elements
    neural_field_u->addInput(gaussKernel_u_u); // self-excitation
    neural_field_u->addInput(noise_kernel_u); // noise

    neural_field_v->addInput(gaussKernel_v_v); // self-excitation
    neural_field_v->addInput(noise_kernel_v); // noise
    neural_field_v->addInput(coupling_u_v); // coupling

    gaussKernel_u_u->addInput(neural_field_u);
    gaussKernel_v_v->addInput(neural_field_v);

    coupling_u_v->addInput(neural_field_u, "activation");

    noise_kernel_u->addInput(noise_u);
    noise_kernel_v->addInput(noise_v);

}

std::shared_ptr<Simulation> DNFarchitecture::getSimulation()
{
    return simulation;
}
