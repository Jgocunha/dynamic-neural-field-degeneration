#include "./user_interface/coupling_window.h"

CouplingWindow::CouplingWindow(const std::shared_ptr<Simulation>& simulation)
	:simulation(simulation)
{
	visualization = std::make_shared<Visualization>(simulation);
}

void CouplingWindow::render()
{
	if(ImGui::Begin("Coupling training wizard"))
	{
		renderCouplingSelector();
		renderPlots();
		renderOperations();
	}
	ImGui::End();
}

void CouplingWindow::renderCouplingSelector()
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
					if (ImGui::Button("Train"))
					{
						fieldCouplingWizard = FieldCouplingWizard{ simulation, elementId };
						/*coupling = std::dynamic_pointer_cast<FieldCoupling>(simulationElement);
						std::shared_ptr<Element> inputElement = simulationElement->getInputs().at(0);
						inputNeuralField = std::dynamic_pointer_cast<NeuralField>(inputElement);
						std::shared_ptr<Element> outputElement = simulation->getElementsThatHaveSpecifiedElementAsInput(simulationElement->getUniqueIdentifier()).at(0);
						outputNeuralField = std::dynamic_pointer_cast<NeuralField>(outputElement);
						visualization->addPlottingData(inputNeuralField->getUniqueIdentifier(), "activation");
						visualization->addPlottingData(outputNeuralField->getUniqueIdentifier(), "activation");
						coupling->resetWeights();
						fieldSize = inputNeuralField->getSize();*/
					}
					ImGui::TreePop();
				}
			}
		}
		ImGui::TreePop();
	}
}

void CouplingWindow::renderPlots()
{
	ImPlot::SetNextAxesLimits(0, 100, -30, 40);
	ImPlotStyle& style = ImPlot::GetStyle();
	style.LineWeight = 3.0f;

	if (ImPlot::BeginPlot("Neural fields activation"))
	{
		ImPlot::SetupAxes("Field position", "Amplitude");
		ImPlot::SetupLegend(ImPlotLocation_NorthEast, ImPlotLegendFlags_Outside);

		uint8_t numOfPlots = visualization->getNumberOfPlots();
		for (int j = 0; j < numOfPlots; j++)
		{
			std::string label = visualization->getPlottingLabel(j);
			std::vector<double> data = *visualization->getPlottingData(j);
			ImPlot::PlotLine(label.c_str(), &data[0], data.size());
		}

		// Check for mouse press event
		checkForMousePress();

		ImPlot::EndPlot();
	}
}

void CouplingWindow::renderOperations()
{
	ImGui::Text("Create target input, by pressing the plot in the appropriate coordinates, or by inputting them.");

	renderAddStimulusButtons();
	renderRemoveStimulusButton();
	renderStimulusFinishedButton();
	renderFinishTrainingButton();
}

void CouplingWindow::checkForMousePress()
{
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImPlot::IsPlotHovered())
	{
		// Retrieve mouse cursor position
		mouseCoordinates.x = ImPlot::GetPlotMousePos(IMPLOT_AUTO).x; // Get x coordinate from ImPlot
		mouseCoordinates.y = ImPlot::GetPlotMousePos(IMPLOT_AUTO).y; // Get y coordinate from ImPlot

		// logger...
		//std::cout << "Mouse coordinates: " << mouseCoordinates.x << " " << mouseCoordinates.y << std::endl;
	}
}

void CouplingWindow::renderAddStimulusButtons()
{
	//if (ImGui::Button("Input selected for input field"))
	//{
	//	// Create stimulus
	//	std::string stimulusId = stimulusInputFieldIdPrefix + std::to_string(inputFieldStimulus.size());
	//	std::shared_ptr<GaussStimulus> stimulus(new GaussStimulus(stimulusId, fieldSize, {3, mouseCoordinates.y, mouseCoordinates.x + offset}));

	//	inputFieldStimulus.push_back(stimulus);

	//	simulation->addElement(stimulus);
	//	simulation->createInteraction(stimulusId, "output", inputNeuralField->getUniqueIdentifier());

	//	simulation->init();
	//	coupling->resetWeights();
	//}

	//// simulate a stimulus for output layer
	//if (ImGui::Button("Input selected for output field"))
	//{
	//	// Create stimulus
	//	std::string stimulusId = stimulusOutputFieldIdPrefix + std::to_string(outputFieldStimulus.size());

	//	std::shared_ptr<GaussStimulus> stimulus(new GaussStimulus(stimulusId, fieldSize, { 3, mouseCoordinates.y, mouseCoordinates.x + offset }));

	//	simulation->addElement(stimulus);
	//	simulation->createInteraction(stimulusId, "output", outputNeuralField->getUniqueIdentifier());

	//	outputFieldStimulus.push_back(stimulus);

	//	simulation->init();
	//	coupling->resetWeights();
	//}
}

void CouplingWindow::renderRemoveStimulusButton()
{
	//if (ImGui::Button("Remove stimulus"))
	//{
	//	// remove stimulus
	//	for (auto& stim : inputFieldStimulus)
	//		simulation->removeElement(stim->getUniqueIdentifier());
	//	inputFieldStimulus.clear();
	//}
}

void CouplingWindow::renderStimulusFinishedButton()
{
	//if (ImGui::Button("Stimulus finished"))
	//{
	//	std::vector<double>* input = simulation->getComponentPtr("field u", "activation");
	//	std::vector<double>* output = simulation->getComponentPtr("field v", "activation");

	//	auto inputRestingLevel = simulation->getComponentPtr("field u", "resting level");
	//	auto outputRestingLevel = simulation->getComponentPtr("field v", "resting level");

	//	// normalize data (remove resting level and normalize between -1 and 1))
	//	*input = normalizeFieldActivation(*input, (*inputRestingLevel)[0]);
	//	*output = normalizeFieldActivation(*output, (*outputRestingLevel)[0]);

	//	// save data
	//	//coupling->writeInputOrOutput(std::string(OUTPUT_DIRECTORY) + "/" + coupling->getUniqueIdentifier() + "_temp_input.txt", input);
	//	//coupling->writeInputOrOutput(std::string(OUTPUT_DIRECTORY) + "/" + coupling->getUniqueIdentifier() + "_temp_output.txt", output);

	//	// remove stimulus
	//	for (auto& stim : outputFieldStimulus)
	//		simulation->removeElement(stim->getUniqueIdentifier());
	//	outputFieldStimulus.clear();
	//	for (auto& stim : inputFieldStimulus)
	//		simulation->removeElement(stim->getUniqueIdentifier());
	//	inputFieldStimulus.clear();


	//	// restart simulation
	//	simulation->init();
	//}
}

void CouplingWindow::renderFinishTrainingButton()
{
	if (ImGui::Button("Finish training"))
		fieldCouplingWizard.trainWeights(500);
}



