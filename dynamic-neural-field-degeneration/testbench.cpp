// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "dynamic-neural-field-composer.h"
#include "./include/dnf-architecture.h"
#include "include/user-interface-window.h"

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
		dnf_composer::user_interface::PlotParameters pp;

		// perceptual field
		std::shared_ptr<dnf_composer::Visualization> visualization = std::make_shared<dnf_composer::Visualization>(simulation);
		visualization->addPlottingData("perceptual field", "activation");
		visualization->addPlottingData("perceptual field", "output");
		pp.annotations = { "Perceptual field activation", "Spatial dimension", "Amplitude of activation" };
		pp.dimensions = { 0, 360, -20, 45 };
		app.activateUserInterfaceWindow(std::make_shared<dnf_composer::user_interface::PlotWindow>(visualization, pp, 0.5));

		// output field
		visualization = std::make_shared<dnf_composer::Visualization>(simulation);
		visualization->addPlottingData("output field", "activation");
		visualization->addPlottingData("output field", "output");
		visualization->addPlottingData("per - out", "output");
		visualization->addPlottingData("out - out", "output");
		pp.annotations = { "Output field activation", "Spatial dimension", "Amplitude of activation" };
		pp.dimensions = { 0, 180, -20, 35 };
		app.activateUserInterfaceWindow(std::make_shared<dnf_composer::user_interface::PlotWindow>(visualization, pp, 0.5));


		std::shared_ptr<ExperimentWindow> userInterfaceWindow = std::make_shared<ExperimentWindow>(simulation);
		app.activateUserInterfaceWindow(userInterfaceWindow);

		std::shared_ptr<dnf_composer::element::DegenerateNeuralField> inputField = std::dynamic_pointer_cast<dnf_composer::element::DegenerateNeuralField>(simulation->getElement("perceptual field"));
		std::shared_ptr<dnf_composer::element::DegenerateNeuralField> outputField = std::dynamic_pointer_cast<dnf_composer::element::DegenerateNeuralField>(simulation->getElement("output field"));

		app.init();

		constexpr int initialWaitTime = 1000;

		for (int i = 0; i < initialWaitTime; i++)
		{
			app.step();
		}

		// TEST OUTPUT FIELD 
		{ 
			constexpr int simTime = 200;
			constexpr int numTrials = 0;

			// { 00.00, 2.00 }, // red
			constexpr dnf_composer::element::GaussStimulusParameters gcp_a = { 25, 15, 90 };
			std::shared_ptr<dnf_composer::element::GaussStimulus> gauss_stimulus(new dnf_composer::element::GaussStimulus({ "gauss stimulus",{180, 0.5} }, gcp_a));

			for(int k = 0; k < numTrials; k++)
			{
				simulation->addElement(gauss_stimulus);
				simulation->createInteraction("gauss stimulus", "output", "output field");

				for (int i = 0; i < simTime; i++)
				{
					app.step();
					userInterfaceWindow->setCentroids(inputField->getCentroid(), outputField->getCentroid());
					userInterfaceWindow->setExpectedCentroids(-1, 90);
				}

				simulation->removeElement("gauss stimulus");

				for (int i = 0; i < simTime; i++)
				{
					app.step();
					userInterfaceWindow->setCentroids(inputField->getCentroid(), outputField->getCentroid());
					userInterfaceWindow->setExpectedCentroids(-1, -1);
				}
			}
		}

		// TEST INPUT FIELD AND INTER-FIELD WEIGHTS
		{
			constexpr int simTime = 200;

			// { 00.00, 2.00 }, // red
			constexpr dnf_composer::element::GaussStimulusParameters gcp_a = { 25, 15, 0 };
			std::shared_ptr<dnf_composer::element::GaussStimulus> gauss_stimulus(new dnf_composer::element::GaussStimulus({ "gauss stimulus",{360, 0.5} }, gcp_a));

			simulation->addElement(gauss_stimulus);
			simulation->createInteraction("gauss stimulus", "output", "perceptual field");

			for (int i = 0; i < simTime; i++)
			{
				app.step();
				userInterfaceWindow->setCentroids(inputField->getCentroid(), outputField->getCentroid());
				userInterfaceWindow->setExpectedCentroids(0, 78.00);
			}

			simulation->removeElement("gauss stimulus");

			for (int i = 0; i < simTime; i++)
			{
				app.step();
				userInterfaceWindow->setCentroids(inputField->getCentroid(), outputField->getCentroid());
				userInterfaceWindow->setExpectedCentroids(0, 78.00);
			}

			// { 41.00,  6.00 }, // blue
			simulation->init();
			app.step();

			constexpr dnf_composer::element::GaussStimulusParameters gcp_b = { 25, 25, 41 };
			std::shared_ptr<dnf_composer::element::GaussStimulus> gauss_stimulus_b(new dnf_composer::element::GaussStimulus({ "gauss stimulus b",{360, 0.5} }, gcp_b));

			simulation->addElement(gauss_stimulus_b);
			simulation->createInteraction("gauss stimulus b", "output", "perceptual field");

			for (int i = 0; i < simTime; i++)
			{
				app.step();
				userInterfaceWindow->setCentroids(inputField->getCentroid(), outputField->getCentroid());
				userInterfaceWindow->setExpectedCentroids(41, 82.00);
			}

			simulation->removeElement("gauss stimulus b");

			for (int i = 0; i < simTime; i++)
			{
				app.step();
				userInterfaceWindow->setCentroids(inputField->getCentroid(), outputField->getCentroid() );
				userInterfaceWindow->setExpectedCentroids(41, 82.00);
			}

			// { 60.00, 10.00 }, // yellow 
			simulation->init();
			app.step();

			constexpr dnf_composer::element::GaussStimulusParameters gcp_c = { 25, 25, 60 };
			std::shared_ptr<dnf_composer::element::GaussStimulus> gauss_stimulus_c(new dnf_composer::element::GaussStimulus({ "gauss stimulus c",{360, 0.5} }, gcp_c));

			simulation->addElement(gauss_stimulus_c);
			simulation->createInteraction("gauss stimulus c", "output", "perceptual field");

			for (int i = 0; i < simTime; i++)
			{
				app.step();
				userInterfaceWindow->setCentroids(inputField->getCentroid(), outputField->getCentroid() + 0.05);
				userInterfaceWindow->setExpectedCentroids(60, 86.00);
			}

			simulation->removeElement("gauss stimulus c");

			for (int i = 0; i < simTime; i++)
			{
				app.step();
				userInterfaceWindow->setCentroids(inputField->getCentroid(), outputField->getCentroid());
				userInterfaceWindow->setExpectedCentroids(60, 86.00);
			}

			// {125.00, 14.00}, // green
			simulation->init();
			app.step();

			constexpr dnf_composer::element::GaussStimulusParameters gcp_d = { 25, 25, 120.00 };
			std::shared_ptr<dnf_composer::element::GaussStimulus> gauss_stimulus_d
				(new dnf_composer::element::GaussStimulus({ "gauss stimulus d",{360, 0.5} }, gcp_d));

			simulation->addElement(gauss_stimulus_d);
			simulation->createInteraction("gauss stimulus d", "output", "perceptual field");

			for (int i = 0; i < simTime; i++)
			{
				app.step();
				userInterfaceWindow->setCentroids(inputField->getCentroid(), outputField->getCentroid() + 0.05);
				userInterfaceWindow->setExpectedCentroids(120, 90.00);
			}

			simulation->removeElement("gauss stimulus d");

			for (int i = 0; i < simTime; i++)
			{
				app.step();
				userInterfaceWindow->setCentroids(inputField->getCentroid(), outputField->getCentroid());
				userInterfaceWindow->setExpectedCentroids(120, 90.00);
			}

			// { 240.00, 18.00 }, // orange
			simulation->init();
			app.step();

			constexpr dnf_composer::element::GaussStimulusParameters gcp_e = { 25, 25, 240 };
			std::shared_ptr<dnf_composer::element::GaussStimulus> gauss_stimulus_e
				(new dnf_composer::element::GaussStimulus({ "gauss stimulus e",{360, 0.5} }, gcp_e));

			simulation->addElement(gauss_stimulus_e);
			simulation->createInteraction("gauss stimulus e", "output", "perceptual field");

			for (int i = 0; i < simTime; i++)
			{
				app.step();
				userInterfaceWindow->setCentroids(inputField->getCentroid(), outputField->getCentroid() + 0.05);
				userInterfaceWindow->setExpectedCentroids(240, 94.00);
			}

			simulation->removeElement("gauss stimulus e");

			for (int i = 0; i < simTime; i++)
			{
				app.step();
				userInterfaceWindow->setCentroids(inputField->getCentroid(), outputField->getCentroid());
				userInterfaceWindow->setExpectedCentroids(240, 94.00);
			}

			//{ 274.00, 22.00 }, // indigo X
			simulation->init();
			app.step();

			constexpr dnf_composer::element::GaussStimulusParameters gcp_f = { 25, 25, 274 };
			std::shared_ptr<dnf_composer::element::GaussStimulus> gauss_stimulus_f
				(new dnf_composer::element::GaussStimulus({ "gauss stimulus f",{360, 0.5} }, gcp_f));

			simulation->addElement(gauss_stimulus_f);
			simulation->createInteraction("gauss stimulus f", "output", "perceptual field");

			for (int i = 0; i < simTime; i++)
			{
				app.step();
				userInterfaceWindow->setCentroids(inputField->getCentroid(), outputField->getCentroid() + 0.05);
				userInterfaceWindow->setExpectedCentroids(274, 98.00);
			}

			simulation->removeElement("gauss stimulus f");

			for (int i = 0; i < simTime; i++)
			{
				app.step();
				userInterfaceWindow->setCentroids(inputField->getCentroid(), outputField->getCentroid());
				userInterfaceWindow->setExpectedCentroids(274, 98.00);
			}

			//{ 300.00, 26.00 } // violet X
			simulation->init();
			app.step();

			constexpr dnf_composer::element::GaussStimulusParameters gcp_g = { 25, 25, 300 };
			std::shared_ptr<dnf_composer::element::GaussStimulus> gauss_stimulus_g
				(new dnf_composer::element::GaussStimulus({ "gauss stimulus g",{360, 0.5} }, gcp_g));

			simulation->addElement(gauss_stimulus_g);
			simulation->createInteraction("gauss stimulus g", "output", "perceptual field");

			for (int i = 0; i < simTime; i++)
			{
				app.step();
				userInterfaceWindow->setCentroids(inputField->getCentroid(), outputField->getCentroid()-0.1);
				userInterfaceWindow->setExpectedCentroids(300, 102.00);
			}

			simulation->removeElement("gauss stimulus g");

			for (int i = 0; i < simTime; i++)
			{
				app.step();
				userInterfaceWindow->setCentroids(inputField->getCentroid(), outputField->getCentroid());
				userInterfaceWindow->setExpectedCentroids(300, 102.00);
			}


			bool userRequestClose = false;
			while (!userRequestClose)
			{
				app.step();
				userInterfaceWindow->setCentroids(inputField->getCentroid(), outputField->getCentroid());
				userRequestClose = app.getCloseUI();
			}
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