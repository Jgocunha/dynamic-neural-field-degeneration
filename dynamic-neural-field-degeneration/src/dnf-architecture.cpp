
#include "../include/dnf-architecture.h"

std::shared_ptr<dnf_composer::Simulation> getExperimentSimulation()
{
	// create simulation object
	std::shared_ptr<dnf_composer::Simulation> simulation = std::make_shared<dnf_composer::Simulation>(2, 0, 0);

	constexpr int perceptualFieldSize = 360;
	constexpr int decisionFieldSize = 28;

	// create neural fields
	constexpr dnf_composer::element::ActivationFunctionParameters afp = {dnf_composer::element::ActivationFunctionType::Sigmoid, 10.0, 0 };
	constexpr dnf_composer::element::NeuralFieldParameters nfp1 = { 25, -12 , afp};
	constexpr dnf_composer::element::NeuralFieldParameters nfp2 = { 25, -15 , afp};
	const std::shared_ptr<dnf_composer::DegenerateNeuralField> perceptual_field(new dnf_composer::DegenerateNeuralField("perceptual field", perceptualFieldSize, nfp1));
	const std::shared_ptr<dnf_composer::DegenerateNeuralField> decision_field(new dnf_composer::DegenerateNeuralField("decision field", decisionFieldSize, nfp2));

	simulation->addElement(perceptual_field);
	simulation->addElement(decision_field);

	// create interactions and add them to the simulation
	dnf_composer::element::GaussKernelParameters gkp1;
	gkp1.amplitude = 25;  // self-sustained (without input)
	gkp1.sigma = 5;
	gkp1.amplitudeGlobal = -0.5;
	const std::shared_ptr<dnf_composer::element::GaussKernel> k_per_per(new dnf_composer::element::GaussKernel("per - per", perceptualFieldSize, gkp1)); // self-excitation u-u
	simulation->addElement(k_per_per);

	dnf_composer::element::GaussKernelParameters gkp2;
	gkp2.amplitude = 20;  // self-stabilized (with input)
	gkp2.sigma = 2;
	gkp2.amplitudeGlobal = -0.3;
	const std::shared_ptr<dnf_composer::element::GaussKernel> k_dec_dec(new dnf_composer::element::GaussKernel("dec - dec", decisionFieldSize, gkp2)); // self-excitation v-v
	simulation->addElement(k_dec_dec);

	dnf_composer::element::FieldCouplingParameters fcp;
	fcp.inputFieldSize = perceptualFieldSize;
	fcp.scalar = 0.75;
	fcp.learningRate = 0.01;
	fcp.learningRule = dnf_composer::LearningRule::DELTA_KROGH_HERTZ;
	const std::shared_ptr<dnf_composer::DegenerateFieldCoupling> w_per_dec(
		new dnf_composer::DegenerateFieldCoupling("per - dec", decisionFieldSize, fcp));
	simulation->addElement(w_per_dec);

	// create noise stimulus and noise kernel
	const std::shared_ptr<dnf_composer::element::NormalNoise> noise_per(new dnf_composer::element::NormalNoise("noise per", perceptualFieldSize, { 1 }));
	const std::shared_ptr<dnf_composer::element::NormalNoise> noise_dec(new dnf_composer::element::NormalNoise("noise dec", decisionFieldSize, { 1 }));
	const std::shared_ptr<dnf_composer::element::GaussKernel> noise_kernel_per(new dnf_composer::element::GaussKernel("noise kernel per", perceptualFieldSize, { 0.25, 0.2 }));
	const std::shared_ptr<dnf_composer::element::GaussKernel> noise_kernel_dec(new dnf_composer::element::GaussKernel("noise kernel dec", decisionFieldSize, { 0.25, 0.2 }));

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

	//set up the field coupling wizard
	//FieldCouplingWizard fcpw{ simulation, "per - dec" };

	////// add gaussian inputs
	//double offset = 1.0;
	//GaussStimulusParameters gsp = { 3, 25, 20 };

	//std::vector<std::vector<double>> inputTargetPeaksForCoupling =
	//{
	//	{ 00.00 + offset }, // red
	//	{ 41.00 + offset }, // orange
	//	{ 60.00 + offset }, // yellow
	//	{ 120.00 + offset }, // green
	//	{ 240.00 + offset }, // blue
	//	{ 274.00 + offset }, // indigo
	//	{ 300.00 + offset } // violet
	//};
	//std::vector<std::vector<double>> outputTargetPeaksForCoupling =
	//{
	//	{ 2.00 + offset },
	//	{ 6.00 + offset },
	//	{ 10.00 + offset },
	//	{ 14.00 + offset },
	//	{ 18.00 + offset },
	//	{ 22.00 + offset },
	//	{ 26.00 + offset }
	//};

	//fcpw.setTargetPeakLocationsForNeuralFieldPre(inputTargetPeaksForCoupling);
	//fcpw.setTargetPeakLocationsForNeuralFieldPost(outputTargetPeaksForCoupling);

	//gsp.amplitude = 25;
	//gsp.sigma = 3;

	//fcpw.setGaussStimulusParameters(gsp);

	//fcpw.simulateAssociation();

	//fcpw.trainWeights(500);

	//fcpw.saveWeights();

	return simulation;

}