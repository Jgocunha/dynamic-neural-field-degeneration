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

		std::shared_ptr<dnf_composer::Visualization> visualization = std::make_shared<dnf_composer::Visualization>(simulation);
		visualization->addPlottingData("perceptual field", "activation");
		visualization->addPlottingData("perceptual field", "output");
		visualization->addPlottingData("per - per", "output");

		dnf_composer::user_interface::PlotParameters pp;
		pp.annotations = { "Perceptual field activation", "Spatial dimension", "Amplitude of activation" };
		pp.dimensions = { 0, 720, -30, 30 };
		app.activateUserInterfaceWindow(std::make_shared<dnf_composer::user_interface::PlotWindow>(visualization, pp));

		visualization = std::make_shared<dnf_composer::Visualization>(simulation);
		visualization->addPlottingData("output field", "activation");
		visualization->addPlottingData("output field", "output");
		visualization->addPlottingData("out - out", "output");
		visualization->addPlottingData("per - out", "output");

		pp.annotations = { "Output field activation", "Spatial dimension", "Amplitude of activation" };
		pp.dimensions = { 0, 280, -20, 20 };
		app.activateUserInterfaceWindow(std::make_shared<dnf_composer::user_interface::PlotWindow>(visualization, pp));

		//return 0;

		app.init();

		constexpr int simTime = 200;

		constexpr dnf_composer::element::GaussStimulusParameters gcp_a = { 25, 25, 0 + 1.0 };
		std::shared_ptr<dnf_composer::element::GaussStimulus> gauss_stimulus(new dnf_composer::element::GaussStimulus({ "gauss stimulus",{360, 0.5} }, gcp_a));

		simulation->addElement(gauss_stimulus);
		simulation->createInteraction("gauss stimulus", "output", "perceptual field");

		for (int i = 0; i < simTime; i++)
			app.step();

		simulation->removeElement("gauss stimulus");

		for (int i = 0; i < simTime; i++)
			app.step();

		constexpr dnf_composer::element::GaussStimulusParameters gcp_b = { 25, 30, 274.15 + 1.0 };
		std::shared_ptr<dnf_composer::element::GaussStimulus> gauss_stimulus_b(new dnf_composer::element::GaussStimulus({ "gauss stimulus b", {360, 0.5} }, gcp_b));
		simulation->addElement(gauss_stimulus_b);
		simulation->createInteraction("gauss stimulus b", "output", "perceptual field");

		for (int i = 0; i < simTime; i++)
			app.step();

		simulation->removeElement("gauss stimulus b");

		for (int i = 0; i < simTime; i++)
			app.step();

		simulation->removeElement("perceptual field");

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