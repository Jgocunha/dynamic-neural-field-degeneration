
#pragma once

#include "./user_interface/user_interface_window.h"


#include "./elements/neural_field.h"
#include "./elements/gauss_stimulus.h"
#include "./elements/mexican_hat_kernel.h"
#include "./elements/gauss_kernel.h"
#include "./elements/normal_noise.h"
#include "./elements/field_coupling.h"


class SimulationWindow : public UserInterfaceWindow
{
private:
	std::shared_ptr<Simulation> simulation;
public:
	SimulationWindow(std::shared_ptr<Simulation> simulation);
	void render() override;
	~SimulationWindow() = default;
private:
	void renderStartSimulationButton();
	void renderAddElement();
	void renderSetInteraction();
	void renderRemoveElement();

	void renderElementProperties(const std::pair<int, std::string>& elementId);

	// these functions need to die
	void addElementNeuralField()
	{
		static char id[CHAR_SIZE] = "neural field u";
		ImGui::InputTextWithHint("id", "enter text here", id, IM_ARRAYSIZE(id));
		static double size = 100;
		ImGui::InputDouble("size", &size, 1.0f, 10.0f, "%.2f");
		static double tau = 20;
		ImGui::InputDouble("tau", &tau, 1.0f, 10.0f, "%.2f");
		static double sigmoidSteepness = 1;
		ImGui::InputDouble("sigmoid steepness", &sigmoidSteepness, 1.0f, 10.0f, "%.2f");
		static double restingLevel = -5.0f;
		ImGui::InputDouble("resting level", &restingLevel, 1.0f, 10.0f, "%.2f");

		if (ImGui::Button("Add", { 100.0f, 30.0f }))
		{
			NeuralFieldParameters nfp = { tau, restingLevel };
			ActivationFunctionParameters afp = { ActivationFunctionType::Sigmoid, sigmoidSteepness, 0};
			std::shared_ptr<NeuralField> neuralField = std::make_shared<NeuralField>(id, size, nfp, afp);
			simulation->addElement(neuralField);
		}
	}
	void addElementGaussStimulus()
	{
		static char id[CHAR_SIZE] = "gauss stimulus a";
		ImGui::InputTextWithHint("id", "enter text here", id, IM_ARRAYSIZE(id));
		static double size = 100;
		ImGui::InputDouble("size", &size, 1.0f, 10.0f, "%.2f");
		static double sigma = 5;
		ImGui::InputDouble("sigma", &sigma, 1.0f, 10.0f, "%.2f");
		static double amplitude = 20;
		ImGui::InputDouble("amplitude", &amplitude, 1.0f, 10.0f, "%.2f");
		static double position = 50;
		ImGui::InputDouble("position", &position, 1.0f, 10.0f, "%.2f");

		if (ImGui::Button("Add", { 100.0f, 30.0f }))
		{
			GaussStimulusParameters gsp = {sigma, amplitude, position };
			std::shared_ptr<GaussStimulus> gaussStimulus = std::make_shared<GaussStimulus>(id, size, gsp);
			simulation->addElement(gaussStimulus);
		}
	}
	void addElementFieldCoupling()
	{
		static char id[CHAR_SIZE] = "coupling u - v";
		ImGui::InputTextWithHint("id", "enter text here", id, IM_ARRAYSIZE(id));
		static double size = 100;
		ImGui::InputDouble("size", &size, 1.0f, 10.0f, "%.2f");
		static double sigma = 5;
		ImGui::InputDouble("sigma", &sigma, 1.0f, 10.0f, "%.2f");

		if (ImGui::Button("Add", { 100.0f, 30.0f }))
		{
			FieldCouplingParameters fcp = { sigma };
			std::shared_ptr<FieldCoupling> gaussCoupling = std::make_shared<FieldCoupling>(id, size, size, fcp, LearningRule::DELTA_KROGH_HERTZ);
			simulation->addElement(gaussCoupling);
		}
	}
	void addElementGaussKernel()
	{
		static char id[CHAR_SIZE] = "gauss kernel u -> u";
		ImGui::InputTextWithHint("id", "enter text here", id, IM_ARRAYSIZE(id));
		static double size = 100;
		ImGui::InputDouble("size", &size, 1.0f, 10.0f, "%.2f");
		static double sigma = 20;
		ImGui::InputDouble("sigma", &sigma, 1.0f, 10.0f, "%.2f");
		static double amplitude = 2;
		ImGui::InputDouble("amplitude", &amplitude, 1.0f, 10.0f, "%.2f");

		if (ImGui::Button("Add", { 100.0f, 30.0f }))
		{
			GaussKernelParameters gkp = { sigma, amplitude };
			std::shared_ptr<GaussKernel> gaussKernel = std::make_shared<GaussKernel>(id, size, gkp);
			simulation->addElement(gaussKernel);
		}
	}
	void addElementMexicanHatKernel()
	{
		static char id[30] = "mexican hat kernel u -> u";
		ImGui::InputTextWithHint("id", "enter text here", id, IM_ARRAYSIZE(id));
		static double size = 100;
		ImGui::InputDouble("size", &size, 1.0f, 10.0f, "%.2f");
		static double sigmaExc = 5;
		ImGui::InputDouble("sigmaExc", &sigmaExc, 1.0f, 10.0f, "%.2f");
		static double amplitudeExc = 15;
		ImGui::InputDouble("amplitudeExc", &amplitudeExc, 1.0f, 10.0f, "%.2f");
		static double sigmaInh = 10;
		ImGui::InputDouble("sigmaInh", &sigmaInh, 1.0f, 10.0f, "%.2f");
		static double amplitudeInh = 15;
		ImGui::InputDouble("amplitudeInh", &amplitudeInh, 1.0f, 10.0f, "%.2f");
		static double amplitudeGlobal = 0;
		ImGui::InputDouble("global amplitude", &amplitudeGlobal, 1.0f, 10.0f, "%.2f");

		if (ImGui::Button("Add", { 100.0f, 30.0f }))
		{
			MexicanHatKernelParameters mhkp = { sigmaExc, amplitudeExc, sigmaInh, amplitudeInh, amplitudeGlobal };
			std::shared_ptr<MexicanHatKernel> mexicanHatKernel = std::make_shared<MexicanHatKernel>(id, size, mhkp);
			simulation->addElement(mexicanHatKernel);
		}
	}
	/*void addNormalNoise()
	{
		static char id[CHAR_SIZE] = "field u";
		ImGui::InputTextWithHint("id", "enter text here", id, IM_ARRAYSIZE(id));
		static double size = 100;
		ImGui::InputDouble("size", &size, 1.0f, 10.0f, "%.2f");
		static double tau = 20;
		ImGui::InputDouble("tau", &tau, 1.0f, 10.0f, "%.2f");
		static double sigmoidSteepness = 1;
		ImGui::InputDouble("sigmoid steepness", &sigmoidSteepness, 1.0f, 10.0f, "%.2f");
		static double restingLevel = -5.0f;
		ImGui::InputDouble("resting level", &restingLevel, 1.0f, 10.0f, "%.2f");
		if (ImGui::Button("Add", { 100.0f, 30.0f }))
		{
			std::shared_ptr<NeuralField> neural_field(new NeuralField(id, size, tau, restingLevel, sigmoidSteepness));
			for (const auto& pair : simulations)
				pair.second->addElement(neural_field);
		}
	}
	void addElementneuralField()
	{
		static char id[CHAR_SIZE] = "field u";
		ImGui::InputTextWithHint("id", "enter text here", id, IM_ARRAYSIZE(id));
		static double size = 100;
		ImGui::InputDouble("size", &size, 1.0f, 10.0f, "%.2f");
		static double tau = 20;
		ImGui::InputDouble("tau", &tau, 1.0f, 10.0f, "%.2f");
		static double sigmoidSteepness = 1;
		ImGui::InputDouble("sigmoid steepness", &sigmoidSteepness, 1.0f, 10.0f, "%.2f");
		static double restingLevel = -5.0f;
		ImGui::InputDouble("resting level", &restingLevel, 1.0f, 10.0f, "%.2f");
		if (ImGui::Button("Add", { 100.0f, 30.0f }))
		{
			std::shared_ptr<NeuralField> neural_field(new NeuralField(id, size, tau, restingLevel, sigmoidSteepness));
			for (const auto& pair : simulations)
				pair.second->addElement(neural_field);
		}
	}*/
};