#include "dnfc_handler_recov.h"

namespace experiment
{
	namespace relearning
	{
		DnfcomposerHandler::DnfcomposerHandler()
		{
			simulation = getExperimentSimulation();
			application = std::make_unique<dnf_composer::Application>(simulation, true);

			simulationElements.inputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.inputFieldId));
			simulationElements.outputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.outputFieldId));
			simulationElements.fieldCoupling = std::dynamic_pointer_cast<DegenerateFieldCoupling>(simulation->getElement(simulationParameters.fieldCouplingId));

			simulationElements.fcpw = dnf_composer::LearningWizard{ simulation, "per - out" };
			readPeaksForCoupling();
		}

		DnfcomposerHandler::DnfcomposerHandler(bool isUserInterfaceActive)
		{
			simulationParameters.isUserInterfaceActive = isUserInterfaceActive;

			simulation = getExperimentSimulation();
			application = std::make_unique<dnf_composer::Application>(simulation, simulationParameters.isUserInterfaceActive);

			simulationElements.inputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.inputFieldId));
			simulationElements.outputField = std::dynamic_pointer_cast<DegenerateNeuralField>(simulation->getElement(simulationParameters.outputFieldId));
			simulationElements.fieldCoupling = std::dynamic_pointer_cast<DegenerateFieldCoupling>(simulation->getElement(simulationParameters.fieldCouplingId));

			simulationElements.fcpw = dnf_composer::LearningWizard{ simulation, "per - out" };

			readPeaksForCoupling();
		}

		void DnfcomposerHandler::init()
		{
			if (simulationParameters.isUserInterfaceActive)
				setupUserInterface();
			dnfcomposerThread = std::thread(&DnfcomposerHandler::step, this);
			readCentroidsThread = std::thread(&DnfcomposerHandler::updateFieldCentroids, this);
		}

		void DnfcomposerHandler::step()
		{
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

			if (readCentroidsThread.joinable())
				readCentroidsThread.join();
			application->close();
		}

		void DnfcomposerHandler::close()
		{
			dnfcomposerThread.join();
			readCentroidsThread.join();
		}

		void DnfcomposerHandler::stop()
		{
			hasExperimentFinished = true;
		}

		void DnfcomposerHandler::startSimulation()
		{
			simulation->init();
			wasStartSimulationRequested = false;
		}

		void DnfcomposerHandler::closeSimulation()
		{
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

		void DnfcomposerHandler::setupUserInterface()
		{
			application->addWindow<dnf_composer::user_interface::MainWindow>();
			application->addWindow<dnf_composer::user_interface::HeatmapWindow>();

			std::shared_ptr<dnf_composer::Visualization> visualization = std::make_shared<dnf_composer::Visualization>(simulation);
			visualization->addPlottingData("perceptual field", "activation");

			dnf_composer::user_interface::PlotParameters pp;
			pp.annotations = { "Perceptual field activation", "Spatial dimension", "Amplitude of activation" };
			pp.dimensions = { 0, 360, -25, 45, 0.5 };
			application->addWindow<dnf_composer::user_interface::PlotWindow>(visualization, pp);

			visualization = std::make_shared<dnf_composer::Visualization>(simulation);
			visualization->addPlottingData("output field", "activation");

			pp.annotations = { "Output field activation", "Spatial dimension", "Amplitude of activation" };
			pp.dimensions = { 0, 28, -15, 20, 0.1 };
			application->addWindow<dnf_composer::user_interface::PlotWindow>(visualization, pp);
		}

		void DnfcomposerHandler::setRelearningParameters(const RelearningType& relearningType, const int& numberOfRelearningEpochs,
			const double& learningRate, const int& maximumRelearningCycles, const bool updateAllWeights)
		{
			relearningParameters.type = relearningType;
			relearningParameters.numberOfEpochs = numberOfRelearningEpochs;
			relearningParameters.learningRate = learningRate;
			relearningParameters.updateAllWeights = updateAllWeights;
			simulationElements.fieldCoupling->setUpdateAllWeights(updateAllWeights);
		}

		void DnfcomposerHandler::setDegeneracy(degeneration::ElementDegeneracyType degeneracyType, const std::string& fieldToDegenerate)
		{
			simulationParameters.degeneracyType = degeneracyType;
			simulationParameters.fieldToDegenerate = fieldToDegenerate;
			wasDegenerationRequested = true;
		}

		void DnfcomposerHandler::setExternalInput(const double& position)
		{
			this->simulationParameters.externalInputPosition = position;
			wasExternalInputUpdated = true;
		}

		void DnfcomposerHandler::setRelearning(const int& targetRelearningPositions)
		{
			relearningParameters.targetRelearningPositions = targetRelearningPositions;
			wasRelearningRequested = true;
		}

		void DnfcomposerHandler::setHaveFieldsSettled(bool haveFieldsSettled)
		{
			this->haveFieldsSettled = haveFieldsSettled;
		}

		void DnfcomposerHandler::setHasRelearningFinished(bool hasRelearningFinished)
		{
			this->hasRelearningFinished = hasRelearningFinished;
		}

		void DnfcomposerHandler::setIsUserInterfaceActiveAs(bool isUserInterfaceActive)
		{
			simulationParameters.isUserInterfaceActive = isUserInterfaceActive;
			application->setActivateUserInterfaceAs(isUserInterfaceActive);
		}

		void DnfcomposerHandler::setWasStartSimulationRequested(bool wasStartSimulationRequested)
		{
			this->wasStartSimulationRequested = wasStartSimulationRequested;
		}

		void DnfcomposerHandler::setWasCloseSimulationRequested(bool wasCloseSimulationRequested)
		{
			this->wasCloseSimulationRequested = wasCloseSimulationRequested;
		}

		double DnfcomposerHandler::getInputFieldCentroid() const
		{
			return simulationParameters.inputFieldCentroid;
		}

		double DnfcomposerHandler::getOutputFieldCentroid() const
		{
			return simulationParameters.outputFieldCentroid;
		}

		bool DnfcomposerHandler::getHaveFieldsSettled() const
		{
			return haveFieldsSettled;
		}

		bool DnfcomposerHandler::getHasRelearningFinished() const
		{
			return hasRelearningFinished;
		}

		std::shared_ptr<ExperimentWindow> DnfcomposerHandler::getUserInterfaceWindow()
		{
			return userInterfaceWindow;
		}

		void DnfcomposerHandler::setIncrementOfDegenerationPercentage(double percentage)
		{
			simulationParameters.incrementOfDegenerationInPercentage = percentage;
		}

		void DnfcomposerHandler::setInitialNumberOfElementsToDegenerate(int count) const
		{
			simulationElements.inputField->setNumNeuronsToDegenerate(count);
			simulationElements.outputField->setNumNeuronsToDegenerate(count);
			simulationElements.fieldCoupling->setNumWeightsToDegenerate(count);
		}

		void DnfcomposerHandler::setNumberOfElementsToDegenerate() const
		{
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
					log(dnf_composer::tools::logger::INFO, "(relearning-experiment) Number of pre-synaptic neurons to degenerate in each iteration: " + std::to_string(numberOfElements) + ".");
				}
				if (simulationParameters.fieldToDegenerate == "output")
				{
					// Perform the calculation 			// Round the result to the nearest integer
					floatingNumberOfElements = static_cast<double>(simulationElements.outputField->getSize()) / 100 * simulationParameters.incrementOfDegenerationInPercentage;
					numberOfElements = static_cast<int>(std::round(floatingNumberOfElements));
					simulationElements.outputField->setNumNeuronsToDegenerate(numberOfElements);
					log(dnf_composer::tools::logger::INFO, "(relearning-experiment) Number of post-synaptic neurons to degenerate in each iteration: " + std::to_string(numberOfElements) + ".");
				}
				break;

			case degeneration::ElementDegeneracyType::WEIGHTS_DEACTIVATE:
			case degeneration::ElementDegeneracyType::WEIGHTS_RANDOMIZE:
			case degeneration::ElementDegeneracyType::WEIGHTS_REDUCE:
				// Perform the calculation 			// Round the result to the nearest integer
				floatingNumberOfElements = static_cast<double>(simulationElements.inputField->getSize() * simulationElements.outputField->getSize()) / 100 * simulationParameters.incrementOfDegenerationInPercentage;
				numberOfElements = static_cast<int>(std::round(floatingNumberOfElements));
				simulationElements.fieldCoupling->setNumWeightsToDegenerate(numberOfElements);
				log(dnf_composer::tools::logger::INFO, "(relearning-experiment) Number of inter-synaptic connections to degenerate in each iteration: " + std::to_string(numberOfElements) + ".");
				break;
			default:
				break;
			}
		}

		int DnfcomposerHandler::getNumberOfDegeneratedElements() const
		{
			return simulationElements.fieldCoupling->getNumIndicesForDegeneration();
		}

		void DnfcomposerHandler::activateDegeneration()
		{
			switch (simulationParameters.degeneracyType)
			{
			case degeneration::ElementDegeneracyType::NEURONS_DEACTIVATE:
				if (simulationParameters.fieldToDegenerate == "perceptual")
				{
					simulationElements.inputField->setDegeneracyType(simulationParameters.degeneracyType);
					simulationElements.inputField->startDegeneration();
					log(dnf_composer::tools::logger::INFO, "(relearning-experiment) Degenerating the perceptual field.");
				}
				else
				{
					simulationElements.outputField->setDegeneracyType(simulationParameters.degeneracyType);
					simulationElements.outputField->startDegeneration();
					log(dnf_composer::tools::logger::INFO, "(relearning-experiment) Degenerating the output field.");
				}
				break;
			case degeneration::ElementDegeneracyType::WEIGHTS_DEACTIVATE:
			case degeneration::ElementDegeneracyType::WEIGHTS_RANDOMIZE:
			case degeneration::ElementDegeneracyType::WEIGHTS_REDUCE:
				simulationElements.fieldCoupling->setDegeneracyType(simulationParameters.degeneracyType);
				simulationElements.fieldCoupling->startDegeneration();
				log(dnf_composer::tools::logger::INFO, "(relearning-experiment) Degenerating the field coupling.");
				break;
			default:
				break;
			}

			waitForFieldsToSettle();

			haveFieldsSettled = true;
			wasDegenerationRequested = false;
		}

		void DnfcomposerHandler::activateRelearning()
		{
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
			simulationElements.fcpw.trainWeights(relearningParameters.numberOfEpochs);

			wasRelearningRequested = false;
			hasRelearningFinished = true;
		}

		void DnfcomposerHandler::updateExternalInput()
		{
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
			wasUpdateWeightsRequested = true;
		}

		void DnfcomposerHandler::updateWeights()
		{
			simulationElements.fieldCoupling->readWeights();
			wasUpdateWeightsRequested = false;
		}

		void DnfcomposerHandler::setDataFilePath(const std::string& filePath)
		{
			simulationElements.fieldCoupling->setWeightsFilePath(filePath);
			simulationElements.fcpw.setDataFilePath(filePath);
		}

		void DnfcomposerHandler::saveWeights()
		{
			wasSaveWeightsRequested = true;
		}

		void DnfcomposerHandler::saveWeightsToFile()
		{
			simulationElements.fieldCoupling->saveWeights();
			wasSaveWeightsRequested = false;
		}

		void DnfcomposerHandler::waitForFieldsToSettle() const
		{
			for (int i = 0; i < simulationParameters.timeForFieldToSettle; i++)
				application->step();
		}

		void DnfcomposerHandler::allCasesRelearning()
		{
			simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPre(inputTargetPeaksForCoupling);
			simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPost(outputTargetPeaksForCoupling);
		}

		void DnfcomposerHandler::onlyDegeneratedCasesRelearning()
		{
			std::vector<std::vector<double>> inputSelected;
			std::vector<std::vector<double>> outputSelected;

			std::ostringstream logStream;

			logStream << "(relearning-experiment) Target behaviors to relearn ";

			for (int i = 0; i < inputTargetPeaksForCoupling.size(); i++)
			{
				if (!(relearningParameters.targetRelearningPositions & (1 << i)))
				{
					int index = 0;
					{
						if (i == 0)
							index = 5; // orange {41, 6}
						else if (i == 1)
							index = 4; // violet {300, 26}
						else if (i == 2)
							index = 3; // indigo {274, 22}
						else if (i == 3)
							index = 1; // green {120, 14}
						else if (i == 4)
							index = 2; // blue {240, 18}
						else if (i == 5)
							index = 6; // yellow {60, 10}
						else if (i == 6)
							index = 0; // red {0, 2}
					}
					inputSelected.push_back(inputTargetPeaksForCoupling[index]);
					outputSelected.push_back(outputTargetPeaksForCoupling[index]);
					logStream << outputTargetPeaksForCoupling[index][0] - offset << " ";
				}
			}

			logStream << std::endl;

			log(dnf_composer::tools::logger::INFO, logStream.str());
			std::cout << logStream.str();

			simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPre(inputSelected);
			simulationElements.fcpw.setTargetPeakLocationsForNeuralFieldPost(outputSelected);
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