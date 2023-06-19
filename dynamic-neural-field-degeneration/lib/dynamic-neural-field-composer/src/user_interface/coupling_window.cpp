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
						coupling = std::dynamic_pointer_cast<FieldCoupling>(simulationElement);
						auto inputNeuralField = simulationElement->getInputs().at(0);
						auto outputNeuralField = simulation->getElementsThatHaveSpecifiedElementAsInput(simulationElement->getUniqueIdentifier()).at(0);
						visualization->addPlottingData(inputNeuralField->getUniqueIdentifier(), "activation");
						visualization->addPlottingData(outputNeuralField->getUniqueIdentifier(), "activation");
						coupling->resetWeights();
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
	ImGui::Text("Create target input, by pressing the plot in the appropriate position and amplitude.");
	
	std::string registeredInputsFilename = "temp_input.txt";
	std::string registeredOutputsFilename = "temp_output.txt";

	static int fieldSize = 100;

	static std::vector<std::shared_ptr<GaussStimulus>> inputFieldStimulus;
	static std::vector<std::shared_ptr<GaussStimulus>> outputFieldStimulus;
	static std::string stimulusInputFieldIdPrefix = "training stim u ";
	static std::string stimulusOutputFieldIdPrefix = "training stim v ";

	// simulate a stimulus for input layer
	if (ImGui::Button("Input selected for neural field u"))
	{
		// Create stimulus
		std::string stimulusId = stimulusInputFieldIdPrefix + std::to_string(inputFieldStimulus.size());
		std::shared_ptr<GaussStimulus> stimulus(new GaussStimulus(stimulusId, fieldSize, {3, mouseCoordinates.y, mouseCoordinates.x}));
		inputFieldStimulus.push_back(stimulus);

		simulation->addElement(stimulus);
		simulation->createInteraction(stimulusId, "output", "field u");

		simulation->init();
		coupling->resetWeights();
	}

	// simulate a stimulus for output layer
	if (ImGui::Button("Input selected for neural field v"))
	{
		// Create stimulus
		std::string stimulusId = stimulusOutputFieldIdPrefix + std::to_string(outputFieldStimulus.size());
		std::shared_ptr<GaussStimulus> stimulus(new GaussStimulus(stimulusId, fieldSize, {3, mouseCoordinates.y, mouseCoordinates.x}));
		
		simulation->addElement(stimulus);
		simulation->createInteraction(stimulusId, "output", "field v");
		
		outputFieldStimulus.push_back(stimulus);

		simulation->init();
		coupling->resetWeights();
	}

	if (ImGui::Button("Remove stimulus"))
	{
		// remove stimulus
		for (auto& stim : inputFieldStimulus)
			simulation->removeElement(stim->getUniqueIdentifier());
		inputFieldStimulus.clear();
	}


	// save input and output data, restart simulation and remove stimulus
	if (ImGui::Button("Stimulus finished"))
	{
		std::vector<double>* input = simulation->getComponentPtr("field u", "activation");
		std::vector<double>* output = simulation->getComponentPtr("field v", "activation");

		auto inputRestingLevel = simulation->getComponentPtr("field u", "resting level");
		auto outputRestingLevel = simulation->getComponentPtr("field v", "resting level");

		// normalize data (remove resting level and normalize between -1 and 1))
		*input = normalizeFieldActivation(*input, (*inputRestingLevel)[0]);
		*output = normalizeFieldActivation(*output, (*outputRestingLevel)[0]);

		// save data
		coupling->writeInputOrOutput("temp_input.txt", input);
		coupling->writeInputOrOutput("temp_output.txt", output);

		// remove stimulus
		for (auto& stim : outputFieldStimulus)
			simulation->removeElement(stim->getUniqueIdentifier());
		outputFieldStimulus.clear();
		for (auto& stim : inputFieldStimulus)
			simulation->removeElement(stim->getUniqueIdentifier());
		inputFieldStimulus.clear();


		// restart simulation
		simulation->init();
	}

	// read data, train the weights for X iterations, save the weights and reset the coupling
	if (ImGui::Button("Finish training"))
	{
		coupling->trainWeights("temp_input.txt", "temp_output.txt", 500);
		simulation->init();                
	}
}

void CouplingWindow::checkForMousePress()
{
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImPlot::IsPlotHovered())
	{
		// Retrieve mouse cursor position
		mouseCoordinates.x = ImPlot::GetPlotMousePos(IMPLOT_AUTO).x; // Get x coordinate from ImPlot
		mouseCoordinates.y = ImPlot::GetPlotMousePos(IMPLOT_AUTO).y; // Get y coordinate from ImPlot

		// logger...
		std::cout << "Mouse coordinates: " << mouseCoordinates.x << " " << mouseCoordinates.y << std::endl;
	}
}

std::vector<double> CouplingWindow::normalizeFieldActivation(std::vector<double>& vec, const double& restingLevel)
{
	// this removes the resting level
	// the code works without this  
	// but results are better this way
	//for (double& val : vec)
		//val += restingLevel;

	//int safetyFactor = 20;

	// Find the minimum and maximum values in the vector
	//double maxVal = *std::max_element(vec.begin(), vec.end()) + safetyFactor;
	//double minVal = *std::min_element(vec.begin(), vec.end()) - safetyFactor;
	//double minVal = -2;

	double maxVal = 20;
	double minVal = -30;


	// Normalize the vector
	std::vector<double> normalizedVec;
	for (const double& val : vec)
	{
		double normalized_val = (val - minVal) / (maxVal - minVal) * 2.0 - 1.0;
		normalizedVec.push_back(normalized_val);
	}

	return normalizedVec;
}