#include "examples.h"

std::shared_ptr<Simulation> test_DegeneracyOneLayerFieldModel()
{
    // shared parameters
    double fieldSize = 100;
    double simTime = 500;

    // create simulator object
    std::shared_ptr<Simulation> sim = std::make_shared<Simulation>(5, 0, 0);

    // add input
    GaussStimulusParameters gsp = { 3, 10, 20 };
    gsp.position = 30;
    std::shared_ptr<GaussStimulus> stimulus_a(new GaussStimulus("gauss stimulus a", fieldSize, gsp));
    //std::shared_ptr<GaussStimulus> stimulus_b(new GaussStimulus("gauss stimulus b", fieldSize, gsp));
    //gsp.position = 60;
    //std::shared_ptr<GaussStimulus> stimulus_c(new GaussStimulus("gauss stimulus c", fieldSize, gsp));
    //gsp.position = 80;
    //std::shared_ptr<GaussStimulus> stimulus_d(new GaussStimulus("gauss stimulus d", fieldSize, gsp));

    sim->addElement(stimulus_a);
    
    // create neural fields
    ActivationFunctionParameters afp = { ActivationFunctionType::Heaviside, 0.0, 0.2 };
    NeuralFieldParameters nfp1 = { 20, -10};
    NeuralFieldParameters nfp2 = { 20, -5};
    std::shared_ptr<DegenerateNeuralField> neural_field_u(new DegenerateNeuralField("field u", fieldSize, nfp1, afp));
    std::shared_ptr<DegenerateNeuralField> neural_field_v(new DegenerateNeuralField("field v", fieldSize, nfp2, afp));

    sim->addElement(neural_field_u);
    sim->addElement(neural_field_v);

    // create interactions and add them to the simulation
    //GaussKernelParameters gkp1;
    //gkp1.amplitude = 10; // self-sustained (in the absence of input)
    //gkp1.sigma = 4;
    //std::shared_ptr<GaussKernel> gaussKernel_u_u(new GaussKernel("u - u", fieldSize, gkp1)); // self-excitation u-u
    //sim->addElement(gaussKernel_u_u);

    MexicanHatKernelParameters mhkp1 = { 4, 15, 10, 15, 0 , 0, 5 };
    mhkp1.amplitudeExc = 32; // self-sustained (in the absence of input)
    //mhkp1.sigmaExc = 2;
    //mhkp1.amplitudeInh = 10;
   // mhkp1.sigmaInh = 8;
    //mhkp1.amplitudeGlobal = -1;
    std::shared_ptr<MexicanHatKernel> mexicanHatKernel_u_u(new MexicanHatKernel("u - u", fieldSize, mhkp1)); // self-excitation u-u
    sim->addElement(mexicanHatKernel_u_u);

    GaussKernelParameters gkp2;
    gkp2.amplitude = 6;  // self-stabilized (with input)
    gkp2.sigma = 5;
    std::shared_ptr<GaussKernel> gaussKernel_v_v(new GaussKernel("v - v", fieldSize, gkp2)); // self-excitation v-v
    sim->addElement(gaussKernel_v_v);

    //std::shared_ptr<FieldCoupling> coupling_u_v(new FieldCoupling("u - v", fieldSize, { 0.95, 0.1 }));
    std::shared_ptr<DegenerateFieldCoupling> coupling_u_v(new DegenerateFieldCoupling("u - v", fieldSize, { 0.80, 0.1 }));
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
    neural_field_u->addInput(mexicanHatKernel_u_u); // self-excitation
    neural_field_u->addInput(noise_kernel_u); // noise

    neural_field_v->addInput(gaussKernel_v_v); // self-excitation
    neural_field_v->addInput(noise_kernel_v); // noise
    neural_field_v->addInput(coupling_u_v); // coupling

    mexicanHatKernel_u_u->addInput(neural_field_u);
    gaussKernel_v_v->addInput(neural_field_v);

    coupling_u_v->addInput(neural_field_u, "activation");

    noise_kernel_u->addInput(noise_u);
    noise_kernel_v->addInput(noise_v);

    //coupling_u_v->trainWeights("temp_input.txt", "temp_output.txt", 1000);

    //neural_field_u->addInput(stimulus_a);
    //gaussKernel_u_u->setDegeneracy(true);
    //coupling_u_v->setDegeneracy(true, DegeneracyType::ZERO_VALUES);

    return sim;
}
