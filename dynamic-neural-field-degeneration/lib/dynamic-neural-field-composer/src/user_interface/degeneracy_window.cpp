#include "./user_interface/degeneracy_window.h"

DegeneracyWindow::DegeneracyWindow(const std::shared_ptr<Simulation>& simulation)
	:simulation(simulation)
{
	couplingSelected = false;

	inputField = std::dynamic_pointer_cast<NeuralField>(simulation->getElement("field u"));
	outputField = std::dynamic_pointer_cast<NeuralField>(simulation->getElement("field v"));
	coupling = std::dynamic_pointer_cast<FieldCoupling>(simulation->getElement("u - v"));
	// hardcoded
	gaussKernelInputField = std::dynamic_pointer_cast<GaussKernel>(simulation->getElement("u - u"));
	gaussKernelOutputField = std::dynamic_pointer_cast<GaussKernel>(simulation->getElement("v - v"));
	
	couplingSelected = true;
}

void DegeneracyWindow::render()
{
	if (ImGui::Begin("Centroid window"))
	{

		if (couplingSelected)
		{
			//renderDegeneracyOptions();
			renderFieldCentroids();
			//renderFieldCentroidsPlotOverSimulationIterations();
		}
		else
			renderCouplingSelector();

	}
	ImGui::End();
}

void DegeneracyWindow::renderCouplingSelector()
{
	if (ImGui::TreeNode("Available gauss couplings"))
	{
		static int numberOfElementsInSimulation = simulation->getNumberOfElements();
		for (int i = 0; i < numberOfElementsInSimulation; i++)
		{
			auto simulationElement = simulation->getElement(i);
			ElementLabel elementLabel = simulationElement->getLabel();

			if (elementLabel == ElementLabel::FIELD_COUPLING)
			{
				std::string elementId = simulationElement->getUniqueIdentifier();
				if (ImGui::TreeNode(elementId.c_str()))
				{
					if (ImGui::Button("Select"))
					{
						couplingSelected = true;
						inputField = std::dynamic_pointer_cast<NeuralField>(simulationElement->getInputs().at(0));
						outputField = std::dynamic_pointer_cast<NeuralField>(simulation->getElementsThatHaveSpecifiedElementAsInput(simulationElement->getUniqueIdentifier()).at(0));
						coupling = std::dynamic_pointer_cast<FieldCoupling>(simulationElement);
						// hardcoded
						gaussKernelInputField = std::dynamic_pointer_cast<GaussKernel>(simulation->getElement("u - u"));
						gaussKernelOutputField = std::dynamic_pointer_cast<GaussKernel>(simulation->getElement("v - v"));

					}
					ImGui::TreePop();
				}
			}
		}
		ImGui::TreePop();
	}
}

void DegeneracyWindow::renderDegeneracyOptions()
{
	//// degenerate weights
	//ImGui::Text("Degenerate weights");
	//if (ImGui::Button("Set weights randomly to zero."))
	//	coupling->setDegeneracy(true, DegeneracyType::WEIGHTS_ZERO);
	//if (ImGui::Button("Set weights randomly to random."))
	//	coupling->setDegeneracy(true, DegeneracyType::WEIGHTS_RAND);
	//if (ImGui::Button("Reduce weights randomly by a constant factor."))
	//	coupling->setDegeneracy(true, DegeneracyType::WEIGHTS_REDUCE);

	//// degenerate neural field
	//ImGui::Text("Degenerate neural fields");
	//if (ImGui::Button("'Kill' input neurons."))
	//	inputField->setDegeneracy(true, NeuralFieldDegenerationType::INDIVIDUAL);
	//if (ImGui::Button("'Kill' output neurons."))
	//	outputField->setDegeneracy(true, NeuralFieldDegenerationType::INDIVIDUAL);

	//// degenerate input field kernel
	//ImGui::Text("Degenerate input field kernel");
	//if (ImGui::Button("Set input field kernel randomly to zero."))
	//	gaussKernelInputField->setDegeneracy(true, ZERO_VALUES);
	//if (ImGui::Button("Set input field kernel randomly to random."))
	//	gaussKernelInputField->setDegeneracy(true, RANDOM_VALUES);
	//if (ImGui::Button("Reduce input field kernel randomly by a constant factor."))
	//	gaussKernelInputField->setDegeneracy(true, REDUCE_VALUES);

	//// degenerate output field kernel
	//ImGui::Text("Degenerate output field kernel");
	//if (ImGui::Button("Set output field kernel randomly to zero."))
	//	gaussKernelOutputField->setDegeneracy(true, ZERO_VALUES);
	//if (ImGui::Button("Set output field kernel randomly to random."))
	//	gaussKernelOutputField->setDegeneracy(true, RANDOM_VALUES);
	//if (ImGui::Button("Reduce output field kernel randomly by a constant factor."))
	//	gaussKernelOutputField->setDegeneracy(true, REDUCE_VALUES);
}

void DegeneracyWindow::renderFieldCentroids()
{
	static std::string inputFieldId = inputField->getUniqueIdentifier();
	double inputFieldCentroid = inputField->calculateCentroid();
	ImGui::Text("Centroid of %s : %f", inputFieldId.c_str(), inputFieldCentroid);

	static std::string outputFieldId = outputField->getUniqueIdentifier();
	double outputFieldCentroid = outputField->calculateCentroid();
	ImGui::Text("Centroid of %s : %f", outputFieldId.c_str(), outputFieldCentroid);
}

void DegeneracyWindow::renderFieldCentroidsPlotOverSimulationIterations()
{
	static std::vector<double> outputFieldCentroidsValue;
	static std::vector<double> inputFieldCentroidsValue;

	static int iteration = 0;

	static std::string inputFieldId = inputField->getUniqueIdentifier();
	double inputFieldCentroid = inputField->calculateCentroid();

	static std::string outputFieldId = outputField->getUniqueIdentifier();
	double outputFieldCentroid = outputField->calculateCentroid();
	
	if (inputFieldCentroid > 0.0 && outputFieldCentroid > 0.0)
	{
		inputFieldCentroidsValue.push_back(inputFieldCentroid);
		outputFieldCentroidsValue.push_back(outputFieldCentroid);
	}
	else
	{
		inputFieldCentroidsValue.clear();
		outputFieldCentroidsValue.clear();
		iteration = 0;
	}
	
	ImPlotStyle& style = ImPlot::GetStyle();
	style.LineWeight = 3.0f;

	if (ImPlot::BeginPlot("Plot field centroids over time"))
	{
		ImPlot::SetupAxes("Simulation iterations", "Value of centroid");
		ImPlot::SetupLegend(ImPlotLocation_NorthEast, ImPlotLegendFlags_Outside);

		std::string labelInputField = "Centroid of " + inputFieldId;
		ImPlot::SetupAxisLimits(ImAxis_X1, 0, inputFieldCentroidsValue.size(), ImGuiCond_Always);
		ImPlot::PlotLine(labelInputField.c_str(), &inputFieldCentroidsValue[0], inputFieldCentroidsValue.size());

		//std::string labelOutputField = "Centroid of " + outputFieldId;
		//ImPlot::SetupAxisLimits(ImAxis_X1, 0, outputFieldCentroidsValue.size(), ImGuiCond_Always);
		//ImPlot::PlotLine(labelOutputField.c_str(), &outputFieldCentroidsValue[0], outputFieldCentroidsValue.size());
		iteration++;
		ImPlot::EndPlot();
		
	}

}