
#include "../include/dnf-architecture.h"

std::shared_ptr<Simulation> getExperimentSimulation()
{
	// create simulation object
	std::shared_ptr<Simulation> simulation = std::make_shared<Simulation>(10, 0, 0);

	int perceptualFieldSize = 360;
	int decisionFieldSize = 180;

	// create neural fields
	ActivationFunctionParameters afp = { ActivationFunctionType::Heaviside, 0.0, 0.2 };
	NeuralFieldParameters nfp1 = { 25, -10 };
	NeuralFieldParameters nfp2 = { 20, -10 };
	std::shared_ptr<DegenerateNeuralField> perceptual_field(new DegenerateNeuralField("perceptual field", perceptualFieldSize, nfp1, afp));
	std::shared_ptr<DegenerateNeuralField> decision_field(new DegenerateNeuralField("decision field", decisionFieldSize, nfp2, afp));

	simulation->addElement(perceptual_field);
	simulation->addElement(decision_field);

	// create interactions and add them to the simulation
	GaussKernelParameters gkp1;
	gkp1.amplitude = 35;  // self-sustained (without input)
	gkp1.sigma = 3;
	gkp1.amplitudeGlobal = -1;
	std::shared_ptr<GaussKernel> k_per_per(new GaussKernel("per - per", perceptualFieldSize, gkp1)); // self-excitation u-u
	simulation->addElement(k_per_per);

	GaussKernelParameters gkp2;
	gkp2.amplitude = 15;  // self-stabilized (with input)
	gkp2.sigma = 2;
	gkp1.amplitudeGlobal = -0.5;
	std::shared_ptr<GaussKernel> k_dec_dec(new GaussKernel("dec - dec", decisionFieldSize, gkp2)); // self-excitation v-v
	simulation->addElement(k_dec_dec);

	std::shared_ptr<DegenerateFieldCoupling> w_per_dec(new DegenerateFieldCoupling("per - dec", decisionFieldSize, perceptualFieldSize, { 0.5, 0.01 }, LearningRule::DELTA_KROGH_HERTZ));
	simulation->addElement(w_per_dec);

	// create noise stimulus and noise kernel
	std::shared_ptr<NormalNoise> noise_per(new NormalNoise("noise per", perceptualFieldSize, { 1 }));
	std::shared_ptr<NormalNoise> noise_dec(new NormalNoise("noise dec", decisionFieldSize, { 1 }));
	std::shared_ptr<GaussKernel> noise_kernel_per(new GaussKernel("noise kernel per", perceptualFieldSize, { 0.25, 0.2 }));
	std::shared_ptr<GaussKernel> noise_kernel_dec(new GaussKernel("noise kernel dec", decisionFieldSize, { 0.25, 0.2 }));

	simulation->addElement(noise_per);
	simulation->addElement(noise_dec);
	simulation->addElement(noise_kernel_per);
	simulation->addElement(noise_kernel_dec);

	// define the interactions between the elements
	perceptual_field->addInput(k_per_per); // self-excitation
	perceptual_field->addInput(noise_kernel_per); // noise

	decision_field->addInput(k_dec_dec); // self-excitation
	decision_field->addInput(noise_kernel_dec); // noise
	decision_field->addInput(w_per_dec); // coupling

	k_per_per->addInput(perceptual_field);
	k_dec_dec->addInput(decision_field);

	w_per_dec->addInput(perceptual_field, "activation");

	noise_kernel_per->addInput(noise_per);
	noise_kernel_dec->addInput(noise_dec);

	//GaussStimulusParameters gcp_a = { 3, 15, 274.15 + 1.0 };
	//std::shared_ptr<GaussStimulus> gauss_stimulus(new GaussStimulus("gauss stimulus", perceptualFieldSize, gcp_a));

	//simulation->addElement(gauss_stimulus);
	//perceptual_field->addInput(gauss_stimulus);

	// ==
	// set up the field coupling wizard
	//FieldCouplingWizard fcpw{ simulation, "per - dec" };

	//// add gaussian inputs
	//double offset = 1.0;
	//GaussStimulusParameters gsp = { 3, 15, 20 };

	//std::vector<std::vector<double>> inputTargetPeaksForCoupling =
	//{
	//	{ 00.00 + offset }, // red
	//	{ 40.60 + offset }, // orange
	//	{ 60.00 + offset }, // yellow
	//	{ 120.00 + offset }, // green
	//	{ 240.00 + offset }, // blue
	//	{ 274.15 + offset }, // indigo
	//	{ 281.79 + offset } // violet
	//};
	//std::vector<std::vector<double>> outputTargetPeaksForCoupling =
	//{
	//	{ 15.00 + offset },
	//	{ 40.00 + offset },
	//	{ 65.00 + offset },
	//	{ 90.00 + offset },
	//	{ 115.00 + offset },
	//	{ 140.00 + offset },
	//	{ 165.00 + offset }
	//};

	//fcpw.setTargetPeakLocationsForNeuralFieldPre(inputTargetPeaksForCoupling);
	//fcpw.setTargetPeakLocationsForNeuralFieldPost(outputTargetPeaksForCoupling);

	//gsp.amplitude = 15;
	//gsp.sigma = 3;

	//fcpw.setGaussStimulusParameters(gsp);

	//fcpw.simulateAssociation();

	//fcpw.trainWeights(100);
 
	return simulation;

}
