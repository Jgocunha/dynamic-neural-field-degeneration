
#include "dnf_architecture.h"

#include <elements/normal_noise.h>
#include "degenerate_field_coupling.h"
#include "degenerate_neural_field.h"
#include "wizards/learning_wizard.h"

constexpr bool trainWeights = false;

std::shared_ptr<dnf_composer::Simulation> getExperimentSimulation()
{
	// create simulation object
	std::shared_ptr<dnf_composer::Simulation> simulation = std::make_shared<dnf_composer::Simulation>("robustness and adaptability in DNFs experiment", 30, 0, 0);

	// element common parameters
	dnf_composer::element::ElementSpatialDimensionParameters perceptualFieldSpatialDimensions{ 360, 0.5 };
	dnf_composer::element::ElementSpatialDimensionParameters outputFieldSpatialDimensions{28, 0.1};

	// create neural field
	//const dnf_composer::element::HeavisideFunction activationFunction{ 0 };
	const dnf_composer::element::SigmoidFunction activationFunction{0.0, 10.0};
	const dnf_composer::element::NeuralFieldParameters nfp1 = { 25, -5 , activationFunction };
	const dnf_composer::element::NeuralFieldParameters nfp2 = { 25, -5 , activationFunction };
	const std::shared_ptr<DegenerateNeuralField> perceptual_field
		(new DegenerateNeuralField({ "perceptual field", perceptualFieldSpatialDimensions}, nfp1));
	const std::shared_ptr<DegenerateNeuralField> output_field
		(new DegenerateNeuralField({ "output field", outputFieldSpatialDimensions }, nfp2));

	simulation->addElement(perceptual_field);
	simulation->addElement(output_field);

	// create interactions and add them to the simulation
	dnf_composer::element::GaussKernelParameters gkp1;
	gkp1.amplitude = 40; //33.6 // self-sustained (without input)
	gkp1.width = 25;
	gkp1.amplitudeGlobal = -0.12;
	gkp1.circular = true;
	gkp1.normalized = true;
	const std::shared_ptr<dnf_composer::element::GaussKernel> k_per_per
		(new dnf_composer::element::GaussKernel({ "per - per", perceptualFieldSpatialDimensions }, gkp1)); // self-excitation u-u
	simulation->addElement(k_per_per);

	dnf_composer::element::GaussKernelParameters gkp2;
	gkp2.amplitude = 20;  // self-stabilized (with input) //20
	gkp2.width = 25; // 25
	gkp2.amplitudeGlobal = -0.12;
	gkp2.circular = true;
	gkp2.normalized = true;
	const std::shared_ptr<dnf_composer::element::GaussKernel> k_out_out
		(new dnf_composer::element::GaussKernel({ "out - out", outputFieldSpatialDimensions }, gkp2)); // self-excitation v-v
	simulation->addElement(k_out_out);
	// 0.366
	dnf_composer::element::FieldCouplingParameters fcp{ perceptualFieldSpatialDimensions.size, 0.4, 0.01, dnf_composer::LearningRule::DELTA_KROGH_HERTZ };
	const std::shared_ptr<DegenerateFieldCoupling> w_per_out( 
		new DegenerateFieldCoupling({ "per - out", outputFieldSpatialDimensions }, fcp));
	simulation->addElement(w_per_out);

	// create noise stimulus and noise kernel
	const std::shared_ptr<dnf_composer::element::NormalNoise> noise_per
		(new dnf_composer::element::NormalNoise({ "noise per", perceptualFieldSpatialDimensions }, { 0.01}));
	const std::shared_ptr<dnf_composer::element::NormalNoise> noise_out
		(new dnf_composer::element::NormalNoise({ "noise out", outputFieldSpatialDimensions }, { 0.01 }));
	const std::shared_ptr<dnf_composer::element::GaussKernel> noise_kernel_per
		(new dnf_composer::element::GaussKernel({ "noise kernel per", perceptualFieldSpatialDimensions }, { 0.25, 0.02, 0.0 }));
	const std::shared_ptr<dnf_composer::element::GaussKernel> noise_kernel_out
		(new dnf_composer::element::GaussKernel({ "noise kernel out", outputFieldSpatialDimensions }, { 0.25, 0.02, 0.0 }));

	simulation->addElement(noise_per);
	simulation->addElement(noise_out);
	simulation->addElement(noise_kernel_per);
	simulation->addElement(noise_kernel_out);

	// define the interactions between the elements
	perceptual_field->addInput(k_per_per); // self-excitation
	perceptual_field->addInput(noise_kernel_per); // noise

	output_field->addInput(k_out_out); // self-excitation
	output_field->addInput(noise_kernel_out); // noise
	output_field->addInput(w_per_out); // coupling

	k_per_per->addInput(perceptual_field);
	k_out_out->addInput(output_field);
	w_per_out->addInput(perceptual_field, "activation");

	noise_kernel_per->addInput(noise_per);
	noise_kernel_out->addInput(noise_out);

	if(trainWeights)
	{
		//set up the field coupling wizard
		dnf_composer::LearningWizard fcpw{ simulation, "per - out" };

		constexpr double offset = 0.0;

		std::vector<std::vector<double>> inputTargetPeaksForCoupling =
		{
			{ 00.00 + offset }, // red
			{ 41.00 + offset }, // orange
			{ 60.00 + offset }, // yellow
			{ 120.00 + offset }, // green
			{ 240.00 + offset }, // blue
			{ 274.00 + offset }, // indigo
			{ 300.00 + offset } // violet
		};

		 std::vector<std::vector<double>> outputTargetPeaksForCoupling =
		 {
			{ 2.00 + 0.00 },
			{ 6.00 + 0.00 },
			{ 10.00 + 0.00 },
			{ 14.00 + 0.00 },
			{ 18.00 + 0.00 },
			{ 22.00 + 0.00 },
			{ 26.00 - 0.00 }
		 };

		fcpw.setTargetPeakLocationsForNeuralFieldPre(inputTargetPeaksForCoupling);
		fcpw.setTargetPeakLocationsForNeuralFieldPost(outputTargetPeaksForCoupling);
		fcpw.simulateAssociation();
		fcpw.trainWeights(100);
		fcpw.saveWeights();
	}

	return simulation;
}
