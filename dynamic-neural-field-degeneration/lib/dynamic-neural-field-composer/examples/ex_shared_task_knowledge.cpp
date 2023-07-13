
#include "examples.h"

std::shared_ptr<Simulation> test_SharedTaskKnowledge()
{
    // shared parameters
    uint32_t fieldSize = 100;
    uint32_t simTime = 500;

    // create simulator object
    std::shared_ptr<Simulation> sim = std::make_shared<Simulation>(1, 0, 0);

    // add inputs
    std::shared_ptr<GaussStimulus> stimulus_a(new GaussStimulus("gauss stimulus a", fieldSize,{ 3, 10, 20}));
    std::shared_ptr<GaussStimulus> stimulus_b(new GaussStimulus("gauss stimulus b", fieldSize,{ 3, 10, 50}));
    std::shared_ptr<GaussStimulus> stimulus_c(new GaussStimulus("gauss stimulus c", fieldSize,{ 3, 10, 80 }));

    sim->addElement(stimulus_a);
    sim->addElement(stimulus_b);
    sim->addElement(stimulus_c);

    // create neural fields
    ActivationFunctionParameters afp = { ActivationFunctionType::Sigmoid, 4, 0 };
    std::shared_ptr<NeuralField> neural_field_u(new NeuralField("field u", fieldSize, { 20, -10 }, afp));
    std::shared_ptr<NeuralField> neural_field_v(new NeuralField("field v", fieldSize, { 20, -5 }, afp));

    sim->addElement(neural_field_u);
    sim->addElement(neural_field_v);

    // create interactions and add them to the simulation
    MexicanHatKernelParameters mhkp = { 4, 15, 10, 15, 0 , 0, 5 };
    mhkp.amplitudeExc = 30;
    std::shared_ptr<MexicanHatKernel> mexicanHatKernel_u_u(new MexicanHatKernel("u - u", fieldSize, mhkp));
    GaussKernelParameters gkp;
    gkp.amplitude = 5;
    gkp.sigma = 5;
    std::shared_ptr<GaussKernel> gaussKernel_v_v(new GaussKernel("v - v", fieldSize, gkp));

    std::shared_ptr<FieldCoupling> coupling_u_v(new FieldCoupling("u - v", fieldSize, { 0.75, 0.1}, LearningRule::DELTA_KROGH_HERTZ));

    sim->addElement(mexicanHatKernel_u_u);
    sim->addElement(gaussKernel_v_v);
    sim->addElement(coupling_u_v);

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
    neural_field_u->addInput(mexicanHatKernel_u_u);
    neural_field_u->addInput(noise_kernel_u);

    //neural_field_u->addInput(stimulus_a);
    //neural_field_u->addInput(stimulus_b);


    neural_field_v->addInput(gaussKernel_v_v);
    neural_field_v->addInput(coupling_u_v);
    neural_field_v->addInput(noise_kernel_v);

    mexicanHatKernel_u_u->addInput(neural_field_u);
    gaussKernel_v_v->addInput(neural_field_v);

    coupling_u_v->addInput(neural_field_u, "activation");

    noise_kernel_u->addInput(noise_u);
    noise_kernel_v->addInput(noise_v);

    //coupling_u_v->trainWeights("temp_input.txt", "temp_output.txt", 100);


    return sim;
}