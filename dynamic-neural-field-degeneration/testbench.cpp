// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "dynamic-neural-field-composer.h"
#include "./include/dnf-architecture.h"

// This .cpp file is an example of how you can use the library to create your own DNF simulation.
// This setup runs the application with a GUI.

int main(int argc, char* argv[])
{
	try
	{
		// After defining the simulation, we can create the application.
		auto simulation = getExperimentSimulation();
		// You can run the application without the user interface by setting the second parameter to false.
		constexpr bool activateUserInterface = true;
		const dnf_composer::Application app{ simulation, activateUserInterface };

		// After creating the application, we can add the windows we want to display.
		app.activateUserInterfaceWindow(std::make_shared<dnf_composer::user_interface::SimulationWindow>(simulation));
		app.activateUserInterfaceWindow(std::make_shared<dnf_composer::user_interface::LoggerWindow>());
		dnf_composer::user_interface::PlotParameters plotParameters;
		plotParameters.annotations = { "Plot title", "Spatial dimension", "Amplitude" };
		plotParameters.dimensions = { 0, 360, -30, 40 };
		const std::shared_ptr<dnf_composer::Visualization> visualization_1 = std::make_shared<dnf_composer::Visualization>(simulation);
		visualization_1->addPlottingData("perceptual field", "activation");
		app.activateUserInterfaceWindow(std::make_shared<dnf_composer::user_interface::PlotWindow>(visualization_1, plotParameters));
		plotParameters.annotations = { "Plot title", "Spatial dimension", "Amplitude" };
		plotParameters.dimensions = { 0, 28, -30, 40 };
		const std::shared_ptr<dnf_composer::Visualization> visualization_2 = std::make_shared<dnf_composer::Visualization>(simulation);
		visualization_2->addPlottingData("output field", "activation");
		app.activateUserInterfaceWindow(std::make_shared<dnf_composer::user_interface::PlotWindow>(visualization_2, plotParameters));

		app.init();

		constexpr dnf_composer::element::GaussStimulusParameters gcp_a = { 3, 15, 0 + 1.0 };
		std::shared_ptr<dnf_composer::element::GaussStimulus> gauss_stimulus(new dnf_composer::element::GaussStimulus({ "gauss stimulus", 360 }, gcp_a));

		simulation->addElement(gauss_stimulus);
		simulation->createInteraction("gauss stimulus", "output", "perceptual field");

		for(int i = 0; i < 100; i++)
			app.step();

		simulation->removeElement("gauss stimulus");

		for (int i = 0; i < 100; i++)
			app.step();

		constexpr dnf_composer::element::GaussStimulusParameters gcp_b = { 3, 25, 274.15 + 1.0 };
		std::shared_ptr<dnf_composer::element::GaussStimulus> gauss_stimulus_b(new dnf_composer::element::GaussStimulus({ "gauss stimulus b", 360 }, gcp_b));
		simulation->addElement(gauss_stimulus_b);
		simulation->createInteraction("gauss stimulus b", "output", "perceptual field");

		for (int i = 0; i < 100; i++)
			app.step();

		simulation->removeElement("gauss stimulus b");

		for (int i = 0; i < 100; i++)
			app.step();

		bool userRequestClose = false;
		while (!userRequestClose)
		{
			app.step();
			userRequestClose = app.getCloseUI();
		}
		app.close();
		return 0;
	}
	catch (const dnf_composer::Exception& ex)
	{
		const std::string errorMessage = "Exception: " + std::string(ex.what()) + " ErrorCode: " + std::to_string(static_cast<int>(ex.getErrorCode())) + ". \n";
		log(dnf_composer::LogLevel::FATAL, errorMessage, dnf_composer::LogOutputMode::CONSOLE);
		return static_cast<int>(ex.getErrorCode());
	}
	catch (const std::exception& ex)
	{
		log(dnf_composer::LogLevel::FATAL, "Exception caught: " + std::string(ex.what()) + ". \n", dnf_composer::LogOutputMode::CONSOLE);
		return 1;
	}
	catch (...)
	{
		log(dnf_composer::LogLevel::FATAL, "Unknown exception occurred. \n", dnf_composer::LogOutputMode::CONSOLE);
		return 1;
	}
}