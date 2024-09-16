#include "dnfc_handler_recov.h"

namespace experiment
{
	namespace relearning
	{
		DnfcomposerHandler::DnfcomposerHandler()
		{
			//log(DEBUG,, "DnfcomposerHandler::DnfcomposerHandler()\n");

			simulation = getExperimentSimulation();
			application = std::make_unique<dnf_composer::Application>(simulation, true);

			simulationElements.inputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.inputFieldId));
			simulationElements.outputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.outputFieldId));
			simulationElements.fieldCoupling = std::dynamic_pointer_cast<DegenerateFieldCoupling>(simulation->getElement(simulationParameters.fieldCouplingId));

			simulationElements.fcpw = dnf_composer::LearningWizard{ simulation, "per - out" };

			setupUserInterface();
			readPeaksForCoupling();
		}

		DnfcomposerHandler::DnfcomposerHandler(bool isUserInterfaceActive)
		{
			//log(DEBUG,, "DnfcomposerHandler::DnfcomposerHandler()\n");

			simulationParameters.isUserInterfaceActive = isUserInterfaceActive;

			simulation = getExperimentSimulation();
			application = std::make_unique<dnf_composer::Application>(simulation, simulationParameters.isUserInterfaceActive);

			simulationElements.inputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.inputFieldId));
			simulationElements.outputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.outputFieldId));
			simulationElements.fieldCoupling = std::dynamic_pointer_cast<DegenerateFieldCoupling>(simulation->getElement(simulationParameters.fieldCouplingId));

			simulationElements.fcpw = dnf_composer::LearningWizard{ simulation, "per - out" };

			if (simulationParameters.isUserInterfaceActive)
				setupUserInterface();
			readPeaksForCoupling();
		}

		// init step close and stop methods

		void DnfcomposerHandler::init()
		{
			//log(DEBUG,, "DnfcomposerHandler::init()\n");

			dnfcomposerThread = std::thread(&DnfcomposerHandler::step, this);
			readCentroidsThread = std::thread(&DnfcomposerHandler::updateFieldCentroids, this);
		}

		void DnfcomposerHandler::step()
		{
			//log(DEBUG,, "DnfcomposerHandler::step()\n");

			application->init();

			bool userRequestClose = false;
			while (!userRequestClose && !hasExperimentFinished)
			{
				if (wasDegenerationRequested)
					activateDegeneration();
				else if (wasCloseSimulationRequested)
					closeSimulation();
				else if (wasExternalInputUpdated)
					updateExternalInput();
				else if (wasRelearningRequested)
					activateRelearning();
				//else if (wasStartSimulationRequested)
					//startSimulation();
				else if (wasUpdateWeightsRequested)
					updateWeights();
				else if (wasSaveWeightsRequested)
					saveWeightsToFile();
				else
					application->step();

				Sleep(1);
				if (simulationParameters.isUserInterfaceActive)
					userRequestClose = application->hasUIBeenClosed();
			}

			application->close();
		}

		void DnfcomposerHandler::close()
		{
			//log(DEBUG,, "DnfcomposerHandler::close()\n");

			// Wait for the thread to finish its execution
			dnfcomposerThread.join();
			readCentroidsThread.join();
		}

		void DnfcomposerHandler::stop()
		{
			//log(DEBUG,, "DnfcomposerHandler::stop()\n");

			hasExperimentFinished = true;
		}

		// other methods

		void DnfcomposerHandler::startSimulation()
		{
			//log(DEBUG,, "DnfcomposerHandler::startSimulation()\n");

			simulation->init();
			wasStartSimulationRequested = false;
		}

		void DnfcomposerHandler::closeSimulation()
		{
			//log(DEBUG,, "DnfcomposerHandler::closeSimulation()\n");

			numberOfDegeneratedElements = 0;
			numberOfRelearningCycles = 0;

			simulationElements.fieldCoupling->readWeights();
			simulationElements.fieldCoupling->populateIndicesForDegeneration();

			simulationElements.inputField->clearDegeneration();
			simulationElements.inputField->populateIndicesForDegeneration();

			simulationElements.outputField->clearDegeneration();
			simulationElements.outputField->populateIndicesForDegeneration();

			wasCloseSimulationRequested = false;
		}

		// UI setup

		void DnfcomposerHandler::setupUserInterface()
		{
			application->addWindow<dnf_composer::user_interface::MainWindow>();
			application->addWindow<imgui_kit::LogWindow>();
			application->addWindow<dnf_composer::user_interface::ElementWindow>();
			application->addWindow<dnf_composer::user_interface::SimulationWindow>();
			application->addWindow<dnf_composer::user_interface::HeatmapWindow>();
			application->addWindow<ExperimentWindow>();

			std::shared_ptr<dnf_composer::Visualization> visualization = std::make_shared<dnf_composer::Visualization>(simulation);
			visualization->addPlottingData("perceptual field", "activation");
			visualization->addPlottingData("perceptual field", "output");
			visualization->addPlottingData("per - per", "output");

			dnf_composer::user_interface::PlotParameters pp;
			pp.annotations = { "Perceptual field activation", "Spatial dimension", "Amplitude of activation" };
			pp.dimensions = { 0, 360, -25, 40, 0.5 };
			application->addWindow<dnf_composer::user_interface::PlotWindow>(visualization, pp);

			visualization = std::make_shared<dnf_composer::Visualization>(simulation);
			visualization->addPlottingData("output field", "activation");
			visualization->addPlottingData("output field", "output");
			visualization->addPlottingData("out - out", "output");
			visualization->addPlottingData("per - out", "output");

			pp.annotations = { "Output field activation", "Spatial dimension", "Amplitude of activation" };
			pp.dimensions = { 0, 280, -20, 40, 1.0 };
			application->addWindow<dnf_composer::user_interface::PlotWindow>(visualization, pp);
		}


		// public set methods for UI


		void DnfcomposerHandler::setRelearningParameters(const RelearningType& relearningType, const int& numberOfRelearningEpochs,
			const double& learningRate, const int& maximumRelearningCycles, const bool updateAllWeights)
		{
			//log(DEBUG,, "DnfcomposerHandler::setRelearningParameters()\n");

			relearningParameters.type = relearningType;
			relearningParameters.numberOfEpochs = numberOfRelearningEpochs;
			relearningParameters.learningRate = learningRate;
			relearningParameters.updateAllWeights = updateAllWeights;
			simulationElements.fieldCoupling->setUpdateAllWeights(updateAllWeights);
			}

		// public set methods for control flags

		void DnfcomposerHandler::setDegeneracy(degeneration::ElementDegeneracyType degeneracyType, const std::string& fieldToDegenerate)
		{
			//log(DEBUG,, "DnfcomposerHandler::setDegeneracy()\n");

			simulationParameters.degeneracyType = degeneracyType;
			simulationParameters.fieldToDegenerate = fieldToDegenerate;
			wasDegenerationRequested = true;
		}

		void DnfcomposerHandler::setExternalInput(const double& position)
		{
			//log(DEBUG,, "DnfcomposerHandler::setExternalInput()\n");

			this->simulationParameters.externalInputPosition = position;
			wasExternalInputUpdated = true;
		}

		void DnfcomposerHandler::setRelearning(const int& targetRelearningPositions)
		{
			//log(DEBUG,, "DnfcomposerHandler::setRelearning()\n");

			relearningParameters.targetRelearningPositions = targetRelearningPositions;
			wasRelearningRequested = true;
		}

		void DnfcomposerHandler::setHaveFieldsSettled(bool haveFieldsSettled)
		{
			//log(DEBUG,, "DnfcomposerHandler::setHaveFieldsSettled()\n");

			this->haveFieldsSettled = haveFieldsSettled;
		}

		void DnfcomposerHandler::setHasRelearningFinished(bool hasRelearningFinished)
		{
			//log(DEBUG,, "DnfcomposerHandler::setHasRelearningFinished()\n");

			this->hasRelearningFinished = hasRelearningFinished;
		}

		void DnfcomposerHandler::setIsUserInterfaceActiveAs(bool isUserInterfaceActive) const
		{
			//log(DEBUG,, "DnfcomposerHandler::setIsUserInterfaceActiveAs()\n");

			application->setActivateUserInterfaceAs(isUserInterfaceActive);
		}

		void DnfcomposerHandler::setWasStartSimulationRequested(bool wasStartSimulationRequested)
		{
			//log(DEBUG,, "DnfcomposerHandler::setWasStartSimulationRequested()\n");

			this->wasStartSimulationRequested = wasStartSimulationRequested;
		}

		void DnfcomposerHandler::setWasCloseSimulationRequested(bool wasCloseSimulationRequested)
		{
			//log(DEBUG,, "DnfcomposerHandler::setWasCloseSimulationRequested()\n");

			this->wasCloseSimulationRequested = wasCloseSimulationRequested;
		}

		// public get methods

		double DnfcomposerHandler::getInputFieldCentroid() const
		{
			////log(DEBUG,, "DnfcomposerHandler::getInputFieldCentroid()\n");

			return simulationParameters.inputFieldCentroid;
		}

		double DnfcomposerHandler::getOutputFieldCentroid() const
		{
			//log(dnf_composer::DEBUG, "DnfcomposerHandler::getOutputFieldCentroid()\n");

			return simulationParameters.outputFieldCentroid;
		}

		bool DnfcomposerHandler::getHaveFieldsSettled() const
		{
			//log(dnf_composer::DEBUG, "DnfcomposerHandler::getHaveFieldsSettled()\n");

			return haveFieldsSettled;
		}

		bool DnfcomposerHandler::getHasRelearningFinished() const
		{
			//log(dnf_composer::DEBUG, "DnfcomposerHandler::getHasRelearningFinished()\n");

			return hasRelearningFinished;
		}

		std::shared_ptr<ExperimentWindow> DnfcomposerHandler::getUserInterfaceWindow()
		{
			//log(dnf_composer::DEBUG, "DnfcomposerHandler::getUserInterfaceWindow()\n");

			return userInterfaceWindow;
		}

		// Degeneration

		void DnfcomposerHandler::setIncrementOfDegenerationPercentage(double percentage)
		{
			//log(DEBUG,, "DnfcomposerHandler::setIncrementOfDegenerationPercentage()\n");

			simulationParameters.incrementOfDegenerationInPercentage = percentage;
		}


		void DnfcomposerHandler::setInitialNumberOfElementsToDegenerate(int count) const
		{
			//log(DEBUG,, "DnfcomposerHandler::setInitialNumberOfElementsToDegenerate()\n");

			simulationElements.inputField->setNumNeuronsToDegenerate(count);
			simulationElements.outputField->setNumNeuronsToDegenerate(count);
			simulationElements.fieldCoupling->setNumWeightsToDegenerate(count);
		}


		void DnfcomposerHandler::setNumberOfElementsToDegenerate() const
		{
			//log(DEBUG,, "DnfcomposerHandler::setNumberOfElementsToDegenerate()\n");

			int numberOfElements;
			double floatingNumberOfElements;

			switch (simulationParameters.degeneracyType)
			{
			case degeneration::ElementDegeneracyType::NEURONS_DEACTIVATE:
				if (simulationParameters.fieldToDegenerate == "perceptual")
				{
					// Perform the calculation 			// Round the result to the nearest integer
					floatingNumberOfElements = static_cast<double>(simulationElements.inputField->getSize()) / 100 * simulationParameters.incrementOfDegenerationInPercentage;
					numberOfElements = static_cast<int>(std::round(floatingNumberOfElements));
					simulationElements.inputField->setNumNeuronsToDegenerate(numberOfElements);
					log(dnf_composer::tools::logger::INFO, " Number of pre-synaptic neurons to degenerate in each iteration: " + std::to_string(numberOfElements) + ".\n");
				}
				if (simulationParameters.fieldToDegenerate == "output")
				{
					// Perform the calculation 			// Round the result to the nearest integer
					floatingNumberOfElements = static_cast<double>(simulationElements.outputField->getSize()) / 100 * simulationParameters.incrementOfDegenerationInPercentage;
					numberOfElements = static_cast<int>(std::round(floatingNumberOfElements));
					simulationElements.outputField->setNumNeuronsToDegenerate(numberOfElements);
					log(dnf_composer::tools::logger::INFO, "Number of post-synaptic neurons to degenerate in each iteration: " + std::to_string(numberOfElements) + ".\n");
				}
				break;

			case degeneration::ElementDegeneracyType::WEIGHTS_DEACTIVATE:
			case degeneration::ElementDegeneracyType::WEIGHTS_RANDOMIZE:
			case degeneration::ElementDegeneracyType::WEIGHTS_REDUCE:
				// Perform the calculation 			// Round the result to the nearest integer
				floatingNumberOfElements = static_cast<double>(simulationElements.inputField->getSize() * simulationElements.outputField->getSize()) / 100 * simulationParameters.incrementOfDegenerationInPercentage;
				numberOfElements = static_cast<int>(std::round(floatingNumberOfElements));
				simulationElements.fieldCoupling->setNumWeightsToDegenerate(numberOfElements);
				log(dnf_composer::tools::logger::INFO, " Number of inter-synaptic connections to degenerate in each iteration: " + std::to_string(numberOfElements) + ".\n");
				break;
			default:
				break;
			}
		}

		int DnfcomposerHandler::getNumberOfDegeneratedElements()
		{
			// return etc.
			return simulationElements.fieldCoupling->getNumIndicesForDegeneration();
		}


		void DnfcomposerHandler::activateDegeneration()
		{
			//log(DEBUG,, "DnfcomposerHandler::activateDegeneration()\n");

			switch (simulationParameters.degeneracyType)
			{
			case degeneration::ElementDegeneracyType::NEURONS_DEACTIVATE:
				if (simulationParameters.fieldToDegenerate == "perceptual")
				{
					simulationElements.inputField->setDegeneracyType(simulationParameters.degeneracyType);
					simulationElements.inputField->startDegeneration();
					log(dnf_composer::tools::logger::INFO, "Degenerating the perceptual field.\n");
				}
				else
				{
					simulationElements.outputField->setDegeneracyType(simulationParameters.degeneracyType);
					simulationElements.outputField->startDegeneration();
					log(dnf_composer::tools::logger::INFO, "Degenerating the output field.\n");
				}
				break;
			case degeneration::ElementDegeneracyType::WEIGHTS_DEACTIVATE:
			case degeneration::ElementDegeneracyType::WEIGHTS_RANDOMIZE:
			case degeneration::ElementDegeneracyType::WEIGHTS_REDUCE:
				simulationElements.fieldCoupling->setDegeneracyType(simulationParameters.degeneracyType);
				simulationElements.fieldCoupling->startDegeneration();
				log(dnf_composer::tools::logger::INFO, "Degenerating the field coupling.\n");
				break;
			default:
				break;
			}

			waitForFieldsToSettle();

			haveFieldsSettled = true;
			wasDegenerationRequested = false;
		}

		// Relearning

		void DnfcomposerHandler::activateRelearning()
		{
			//log(DEBUG,, "DnfcomposerHandler::activateRelearning()\n");

			// NOW WE HAVE RELEARNING TYPE, LEARNING RATE, AND NUMBER OF ITERATIONS
			// USE THIS INFORMATION TO TRAIN THE WEIGHTS
			simulationElements.fieldCoupling->setLearningRate(relearningParameters.learningRate);

			// Remove previously written target peak locations from files
			simulationElements.fcpw.clearTargetPeakLocationsFromFiles();

			switch (relearningParameters.type)
			{
			case RelearningType::ALL_CASES:
				allCasesRelearning();
				break;
			case RelearningType::ONLY_DEGENERATED_CASES:
				onlyDegeneratedCasesRelearning();
				break;
			default:
				break;
			}

			simulationElements.fcpw.simulateAssociation();
			//std::cout << "Finished simulating association.\n";

			simulationElements.fcpw.trainWeights(relearningParameters.numberOfEpochs);
			//std::cout << "Finished training weights.\n";

			wasRelearningRequested = false;
			hasRelearningFinished = true;
		}

		void DnfcomposerHandler::updateExternalInput()
		{
			//log(DEBUG,, "DnfcomposerHandler::updateExternalInput()\n");

			static auto kernel = std::dynamic_pointer_cast<dnf_composer::element::GaussKernel>(simulation->getElement("per - per"));
			static auto kernel_width = kernel->getParameters().width;
			static auto kernel_amplitude = kernel->getParameters().amplitude;

			static double offset = 0.0;
			dnf_composer::element::GaussStimulusParameters gsp = { kernel_width, kernel_amplitude, 20 };
			gsp.position = simulationParameters.externalInputPosition + offset;
			const std::shared_ptr<dnf_composer::element::GaussStimulus> stimulus
			(new dnf_composer::element::GaussStimulus({ "stimulus", {simulationElements.inputField->getMaxSpatialDimension(), simulationElements.inputField->getStepSize()} }, gsp));

			simulation->addElement(stimulus);
			stimulus->init();
			simulationElements.inputField->addInput(stimulus);
			waitForFieldsToSettle();

			simulation->removeElement("stimulus");
			waitForFieldsToSettle();

			haveFieldsSettled = true;
			wasExternalInputUpdated = false;
		}

		void DnfcomposerHandler::updateFieldCentroids()
		{
			//log(DEBUG,, "DnfcomposerHandler::updateFieldCentroids()\n");

			bool userRequestClose = false;
			while (!userRequestClose && !hasExperimentFinished)
			{
				simulationParameters.inputFieldCentroid = simulationElements.inputField->getCentroid();
				simulationParameters.outputFieldCentroid = simulationElements.outputField->getCentroid();

				if (simulationParameters.isUserInterfaceActive)
					userRequestClose = application->hasUIBeenClosed();
				Sleep(1);
			}
		}

		void DnfcomposerHandler::readWeights()
		{
			//log(DEBUG,, "DnfcomposerHandler::readWeights()\n");

			wasUpdateWeightsRequested = true;
		}

		void DnfcomposerHandler::updateWeights()
		{
			//log(DEBUG,, "DnfcomposerHandler::updateWeights()\n");

			simulationElements.fieldCoupling->readWeights();
			wasUpdateWeightsRequested = false;
		}


		// other methods

		void DnfcomposerHandler::setDataFilePath(const std::string& filePath)
		{
			//log(DEBUG,, "DnfcomposerHandler::setDataFilePath()\n");

			simulationElements.fieldCoupling->setWeightsFilePath(filePath);
			simulationElements.fcpw.setDataFilePath(filePath);
		}


		void DnfcomposerHandler::saveWeights()
		{
			//log(DEBUG,, "DnfcomposerHandler::saveWeights()\n");

			wasSaveWeightsRequested = true;
		}


		void DnfcomposerHandler::saveWeightsToFile()
		{
			//log(DEBUG,, "DnfcomposerHandler::saveWeightsToFile()\n");

			simulationElements.fieldCoupling->saveWeights();
			wasSaveWeightsRequested = false;
		}

		void DnfcomposerHandler::waitForFieldsToSettle() const
		{
			//log(DEBUG,, "DnfcomposerHandler::waitForFieldsToSettle()\n");

			for (int i = 0; i < simulationParameters.timeForFieldToSettle; i++)
				application->step();
		}

		void DnfcomposerHandler:: allCasesRelearning()
		{
			//log(DEBUG,, "DnfcomposerHandler::allCasesRelearning()\n");

			// add gaussian inputs
			//dnf_composer::element::GaussStimulusParameters gsp = { 3, 35, 20 };

			simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPre(inputTargetPeaksForCoupling);
			simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPost(outputTargetPeaksForCoupling);
			//std::cout << "Finished setting up the field coupling wizard.\n";


			//gsp.amplitude = 35;
			//gsp.sigma = 3;
			//simulationElements.fcpw.setGaussStimulusParameters(gsp);
			////log(dnf_composer::WARNING, "allCasesRelearning() finished setting target peak locations");

			//std::cout << "Finished setting up the gaussian stimulus parameters.\n";

		}

		void DnfcomposerHandler::onlyDegeneratedCasesRelearning()
		{
			//log(dnf_composer::DEBUG, "DnfcomposerHandler::onlyDegeneratedCasesRelearning()\n");

			// add gaussian inputs
			dnf_composer::element::GaussStimulusParameters gsp = { 3, 35, 20 };

			std::vector<std::vector<double>> inputSelected;
			std::vector<std::vector<double>> outputSelected;

			std::ostringstream logStream;


			logStream << "Target behaviors to relearn ";

			for (int i = 0; i < inputTargetPeaksForCoupling.size(); i++)
			{
				if (!(relearningParameters.targetRelearningPositions & (1 << i)))
				{
					int index = 0;
					if (inputTargetPeaksForCoupling.size() == 7)
					{
						index = 6 - i;
						if (index == 2)
							index = 4;
						else
							if (index == 4)
								index = 2;
					}
					else
					{
						if (inputTargetPeaksForCoupling.size() == 1)
							index = 0;
						else
						{
							log(dnf_composer::tools::logger::WARNING, "Automatic indexing system in onlyDegeneratedCasesRelearning() will not work with 2-6 target behaviors.");
						}
					}

					inputSelected.push_back(inputTargetPeaksForCoupling[index]);
					outputSelected.push_back(outputTargetPeaksForCoupling[index]);
					logStream << outputTargetPeaksForCoupling[index][0] - offset << " ";
				}
			}

			logStream << std::endl;

			log(dnf_composer::tools::logger::INFO, logStream.str());

			simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPre(inputSelected);
			simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPost(outputSelected);
			//std::cout << "Finished setting up the field coupling wizard.\n";

			//gsp.amplitude = 35;
			//gsp.sigma = 3;
			//simulationElements.fcpw.setGaussStimulusParameters(gsp);
			////std::cout << "Finished setting up the gaussian stimulus parameters.\n";

		}

		void DnfcomposerHandler::readPeaksForCoupling()
		{
			std::ifstream file(std::string(PROJECT_DIR) + "/hue_to_angle.json");

			if (!file.is_open()) {
				std::cerr << "Error: Could not open the JSON colors file." << std::endl;
			}

			nlohmann::json j;
			file >> j;

			for (auto& [key, value] : j.items()) {

				if (key == "metadata") {
					continue;
				}
				const double hue = std::stod(key); 
				const double angle = value.get<double>();

				inputTargetPeaksForCoupling.push_back({ hue + offset });
				outputTargetPeaksForCoupling.push_back({ angle + offset });
			}

			file.close();
		}

	}
}