// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "./user_interface/simulation_window.h"


namespace dnf_composer
{
	namespace user_interface
	{
		SimulationWindow::SimulationWindow(const std::shared_ptr<Simulation>& simulation)
			: simulation(simulation)
		{
		}

		void SimulationWindow::render()
		{
			if (ImGui::Begin("Simulation control"))
			{
				renderStartSimulationButton();
				renderAddElement();
				renderSetInteraction();
				renderRemoveElement();
				renderLogElementProperties();
			}
			ImGui::End();
		}

		void SimulationWindow::renderStartSimulationButton() const
		{
			if (ImGui::Button("Start simulation", { 200.00f, 35.00f }))
				simulation->init();

			ImGui::SameLine();

			if (ImGui::Button("Restart simulation", { 200.00f, 35.00f }))
				simulation->close();
		}

		void SimulationWindow::renderAddElement() const
		{
			if (ImGui::CollapsingHeader("Add element"))
			{
				for (const auto& pair : element::ElementLabelToString)
					renderElementProperties(pair);
			}
		}

		void SimulationWindow::renderSetInteraction() const
		{
			if (ImGui::CollapsingHeader("Set interactions between elements"))
			{
				const int numberOfElementsInSimulation = simulation->getNumberOfElements();

				for (int i = 0; i < numberOfElementsInSimulation; i++)
				{
					const auto simulationElement = simulation->getElement(i);
					std::string targetElementId = simulationElement->getUniqueName();

					if (ImGui::TreeNode(targetElementId.c_str()))
					{
						static std::string selectedElementId{};
						static int currentElementIdx = 0;
						ImGui::Text("Select the element you want to define as input");
						if (ImGui::BeginListBox("##Element list available as inputs"))
						{
							for (int n = 0; n < numberOfElementsInSimulation; n++)
							{
								const auto element = simulation->getElement(n);
								std::string inputElementId = element->getUniqueName();
								const bool isSelected = (currentElementIdx == n);
								if (ImGui::Selectable(inputElementId.c_str(), isSelected))
								{
									selectedElementId = inputElementId;
									currentElementIdx = n;
								}

								if (isSelected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndListBox();
						}

						if (ImGui::Button("Add", { 100.0f, 30.0f }))
						{
							auto input = simulation->getElement(selectedElementId);
							simulationElement->addInput(input);
						}
						ImGui::TreePop();
					}
				}
			}
		}

		void SimulationWindow::renderRemoveElement() const
		{
			if (ImGui::CollapsingHeader("Remove elements from simulation"))
			{
				int numberOfElementsInSimulation = simulation->getNumberOfElements();

				for (int i = 0; i < numberOfElementsInSimulation; i++)
				{
					const auto simulationElement = simulation->getElement(i);
					std::string elementId = simulationElement->getUniqueName();

					if (ImGui::TreeNode(elementId.c_str()))
					{
						if (ImGui::Button("Remove", { 100.0f, 30.0f }))
						{
							simulation->removeElement(elementId);
							numberOfElementsInSimulation--;
						}
						ImGui::TreePop();
					}
				}
			}
		}

		void SimulationWindow::renderElementProperties(const std::pair<int, std::string>& elementId) const
		{
			if (ImGui::TreeNode(elementId.second.c_str()))
			{
				switch (elementId.first)
				{
				case element::ElementLabel::NEURAL_FIELD:
					addElementNeuralField();
					break;
				case element::ElementLabel::GAUSS_STIMULUS:
					addElementGaussStimulus();
					break;
				case element::ElementLabel::FIELD_COUPLING:
					addElementFieldCoupling();
					break;
				case element::ElementLabel::GAUSS_KERNEL:
					addElementGaussKernel();
					break;
				case element::ElementLabel::MEXICAN_HAT_KERNEL:
					addElementMexicanHatKernel();
					break;
				case element::ElementLabel::NORMAL_NOISE:
					addElementNormalNoise();
					break;
				case element::ElementLabel::GAUSS_FIELD_COUPLING:
					addElementGaussFieldCoupling();
					break;
				default:
					LoggerWindow::addLog(LogLevel::_ERROR, "There is a missing element in the TreeNode in simulation window.");
					break;
				}
				ImGui::TreePop();
			}
		}

		void SimulationWindow::renderLogElementProperties() const
		{
			if (ImGui::CollapsingHeader("Log element parameters"))
			{
				const int numberOfElementsInSimulation = simulation->getNumberOfElements();

				for (int i = 0; i < numberOfElementsInSimulation; i++)
				{
					const auto simulationElement = simulation->getElement(i);
					std::string elementId = simulationElement->getUniqueName();

					if (ImGui::TreeNode(elementId.c_str()))
					{
						if (ImGui::Button("Log", { 100.0f, 30.0f }))
						{
							simulationElement->printParameters();
						}
						ImGui::TreePop();
					}
				}
			}
		}

		void SimulationWindow::addElementNeuralField() const
		{
			static char id[CHAR_SIZE] = "neural field u";
			ImGui::InputTextWithHint("id", "enter text here", id, IM_ARRAYSIZE(id));
			static int size = 100;
			ImGui::InputInt("size", &size, 1.0, 10.0);
			static double tau = 25;
			ImGui::InputDouble("tau", &tau, 1.0f, 10.0f, "%.2f");
			static double sigmoidSteepness = 5.0f;
			ImGui::InputDouble("sigmoid steepness", &sigmoidSteepness, 1.0f, 10.0f, "%.2f");
			static double restingLevel = -10.0f;
			ImGui::InputDouble("resting level", &restingLevel, 1.0f, 10.0f, "%.2f");

			if (ImGui::Button("Add", { 100.0f, 30.0f }))
			{
				element::NeuralFieldParameters nfp = { tau, restingLevel };
				const element::ActivationFunctionParameters afp = {element::ActivationFunctionType::Sigmoid, sigmoidSteepness, 0};
				nfp.activationFunctionParameters = afp;
				const std::shared_ptr<element::NeuralField> neuralField = std::make_shared<element::NeuralField>(id, size, nfp);
				simulation->addElement(neuralField);
			}
		}

		void SimulationWindow::addElementGaussStimulus() const
		{
			static char id[CHAR_SIZE] = "gauss stimulus a";
			ImGui::InputTextWithHint("id", "enter text here", id, IM_ARRAYSIZE(id));
			static int size = 100;
			ImGui::InputInt("size", &size, 1.0, 10.0);
			static double sigma = 5;
			ImGui::InputDouble("sigma", &sigma, 1.0f, 10.0f, "%.2f");
			static double amplitude = 20;
			ImGui::InputDouble("amplitude", &amplitude, 1.0f, 10.0f, "%.2f");
			static double position = 50;
			ImGui::InputDouble("position", &position, 1.0f, 10.0f, "%.2f");


			if (ImGui::Button("Add", { 100.0f, 30.0f }))
			{
				element::GaussStimulusParameters gsp = { sigma, amplitude, position };
				const std::shared_ptr<element::GaussStimulus> gaussStimulus = std::make_shared<element::GaussStimulus>(id, size, gsp);
				simulation->addElement(gaussStimulus);
			}
		}

		void SimulationWindow::addElementNormalNoise() const
		{
			static char id[CHAR_SIZE] = "normal noise a";
			ImGui::InputTextWithHint("id", "enter text here", id, IM_ARRAYSIZE(id));
			static int size = 100;
			ImGui::InputInt("size", &size, 1.0, 10.0);
			static double amplitude = 20;
			ImGui::InputDouble("amplitude", &amplitude, 1.0f, 10.0f, "%.2f");

			if (ImGui::Button("Add", { 100.0f, 30.0f }))
			{
				element::NormalNoiseParameters nnp = { amplitude };
				const std::shared_ptr<element::NormalNoise> normalNoise = std::make_shared<element::NormalNoise>(id, size, nnp);
				element::GaussKernelParameters gkp = { 0.25, 0.2 };
				const std::shared_ptr<element::GaussKernel> gaussKernelNormalNoise = 
					std::make_shared<element::GaussKernel>(std::string(id) + " gauss kernel", size, gkp);
				simulation->addElement(normalNoise);
				simulation->addElement(gaussKernelNormalNoise);
			}
		}

		void SimulationWindow::addElementFieldCoupling() const
		{
		}

		void SimulationWindow::addElementGaussFieldCoupling() const
		{
			
		}

		void SimulationWindow::addElementGaussKernel() const
		{
			static char id[CHAR_SIZE] = "gauss kernel u -> u";
			ImGui::InputTextWithHint("id", "enter text here", id, IM_ARRAYSIZE(id));
			static int size = 100;
			ImGui::InputInt("size", &size, 1.0, 10.0);
			static double sigma = 20;
			ImGui::InputDouble("sigma", &sigma, 1.0f, 10.0f, "%.2f");
			static double amplitude = 2;
			ImGui::InputDouble("amplitude", &amplitude, 1.0f, 10.0f, "%.2f");

			if (ImGui::Button("Add", { 100.0f, 30.0f }))
			{
				element::GaussKernelParameters gkp = { sigma, amplitude };
				const std::shared_ptr<element::GaussKernel> gaussKernel = std::make_shared<element::GaussKernel>(id, size, gkp);
				simulation->addElement(gaussKernel);
			}
		}

		void SimulationWindow::addElementMexicanHatKernel() const
		{
			static char id[CHAR_SIZE] = "mexican hat kernel u -> u";
			ImGui::InputTextWithHint("id", "enter text here", id, IM_ARRAYSIZE(id));
			static int size = 100;
			ImGui::InputInt("size", &size, 1.0, 10.0);
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
				element::MexicanHatKernelParameters mhkp = { sigmaExc, amplitudeExc, sigmaInh, amplitudeInh, amplitudeGlobal };
				const std::shared_ptr<element::MexicanHatKernel> mexicanHatKernel = std::make_shared<element::MexicanHatKernel>(id, size, mhkp);
				simulation->addElement(mexicanHatKernel);
			}
		}

	}
}
