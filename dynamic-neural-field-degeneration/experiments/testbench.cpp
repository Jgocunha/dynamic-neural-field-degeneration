#include <application/application.h>
#include <user_interface/element_window.h>
#include <user_interface/heatmap_window.h>
#include <user_interface/main_window.h>
#include <user_interface/plot_window.h>
#include <user_interface/simulation_window.h>

#include "dnf_architecture.h"

int main()
{
	try
	{
		const auto simulation = getExperimentSimulation();
		const dnf_composer::Application app{ simulation };

		app.addWindow<dnf_composer::user_interface::MainWindow>();
		app.addWindow<imgui_kit::LogWindow>();
		app.addWindow<dnf_composer::user_interface::ElementWindow>();
		app.addWindow<dnf_composer::user_interface::SimulationWindow>();
		app.addWindow<dnf_composer::user_interface::HeatmapWindow>();

		std::shared_ptr<dnf_composer::Visualization> visualization = std::make_shared<dnf_composer::Visualization>(simulation);
		visualization->addPlottingData("perceptual field", "activation");
		visualization->addPlottingData("perceptual field", "output");
		visualization->addPlottingData("per - per", "output");

		dnf_composer::user_interface::PlotParameters pp;
		pp.annotations = { "Perceptual field activation", "Spatial dimension", "Amplitude of activation" };
		pp.dimensions = { 0, 360, -25, 40, 0.5 };
		app.addWindow<dnf_composer::user_interface::PlotWindow>(visualization, pp);

		visualization = std::make_shared<dnf_composer::Visualization>(simulation);
		visualization->addPlottingData("output field", "activation");
		visualization->addPlottingData("output field", "output");
		visualization->addPlottingData("out - out", "output");
		visualization->addPlottingData("per - out", "output");

		pp.annotations = { "Output field activation", "Spatial dimension", "Amplitude of activation" };
		pp.dimensions = { 0, 280, -20, 40, 1.0 };
		app.addWindow<dnf_composer::user_interface::PlotWindow>(visualization, pp);

		app.init();

		bool userRequestClose = false;
		while (!userRequestClose)
		{
			app.step();
			userRequestClose = app.hasUIBeenClosed();
		}
		app.close();
		return 0;
	}
	catch (const dnf_composer::Exception& ex)
	{
		const std::string errorMessage = "Exception: " + std::string(ex.what()) + " ErrorCode: " + std::to_string(static_cast<int>(ex.getErrorCode())) + ". \n";
		log(dnf_composer::tools::logger::LogLevel::FATAL, errorMessage, dnf_composer::tools::logger::LogOutputMode::CONSOLE);
		return static_cast<int>(ex.getErrorCode());
	}
	catch (const std::exception& ex)
	{
		log(dnf_composer::tools::logger::LogLevel::FATAL, "Exception caught: " + std::string(ex.what()) + ". \n", dnf_composer::tools::logger::LogOutputMode::CONSOLE);
		return 1;
	}
	catch (...)
	{
		log(dnf_composer::tools::logger::LogLevel::FATAL, "Unknown exception occurred. \n", dnf_composer::tools::logger::LogOutputMode::CONSOLE);
		return 1;
	}
}

