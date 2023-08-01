#include "./user_interface/simulation_window.h"

SimulationWindow::SimulationWindow(std::shared_ptr<Simulation> simulation)
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
	}
	ImGui::End();
}

void SimulationWindow::renderStartSimulationButton()
{
	if (ImGui::Button("Start simulation"))
	{
		simulation->init();
	}
}

void SimulationWindow::renderAddElement()
{
	if (ImGui::CollapsingHeader("Add element"))
	{
		if (ImGui::TreeNode("Element selector"))
		{
			for (const auto& pair : ElementLabelToString)
				renderElementProperties(pair);
			ImGui::TreePop();
		}
	}
}

void SimulationWindow::renderSetInteraction()
{
	if (ImGui::CollapsingHeader("Set interactions between elements"))
	{
		uint8_t numberOfElementsInSimulation = simulation->getNumberOfElements();

		for (int i = 0; i < numberOfElementsInSimulation; i++)
		{
			auto simulationElement = simulation->getElement(i);
			std::string elementId = simulationElement->getUniqueIdentifier();

			if (ImGui::TreeNode(elementId.c_str()))
			{
				static std::string selectedElementId{};
				static int currentElementIdx = 0;
				if (ImGui::BeginListBox("Active simulation elements"))
				{
					for (int n = 0; n < numberOfElementsInSimulation; n++)
					{
						auto element = simulation->getElement(n);
						std::string elementId = element->getUniqueIdentifier();
						const bool isSelected = (currentElementIdx == n);
						if (ImGui::Selectable(elementId.c_str(), isSelected))
						{
							selectedElementId = elementId;
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

void SimulationWindow::renderRemoveElement()
{
	if (ImGui::CollapsingHeader("Remove elements from simulation"))
	{
		uint8_t numberOfElementsInSimulation = simulation->getNumberOfElements();

		for (int i = 0; i < numberOfElementsInSimulation; i++)
		{
			auto simulationElement = simulation->getElement(i);
			std::string elementId = simulationElement->getUniqueIdentifier();

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

void SimulationWindow::renderElementProperties(const std::pair<int, std::string>& elementId)
{
	if (ImGui::TreeNode(elementId.second.c_str()))
	{
		// for now this is going to have to be a switch case
		// i do not have an idea to make this less copy pasty code
		switch (elementId.first)
		{
		case ElementLabel::NEURAL_FIELD:
			addElementNeuralField();
			break;
		case ElementLabel::GAUSS_STIMULUS:
			addElementGaussStimulus();
			break;
		case ElementLabel::FIELD_COUPLING:
			addElementFieldCoupling();
			break;
		case ElementLabel::GAUSS_KERNEL:
			addElementGaussKernel();
			break;
		case ElementLabel::MEXICAN_HAT_KERNEL:
			addElementMexicanHatKernel();
			break;
		case ElementLabel::NORMAL_NOISE:
			// normal noise to do
			break;
		case ElementLabel::SUM_DIMENSION:
			// sum dimension to do
			break;
		default:
			std::cout << "There is a missing element in the TreeNode\n";
			std::cout << "ImGuiInterface::renderAddElementTreeNode.\n";
			break;
		}
		ImGui::TreePop();
	}
}
