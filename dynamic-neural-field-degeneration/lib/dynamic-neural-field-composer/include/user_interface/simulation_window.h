
#pragma once

#include "./user_interface/user_interface_window.h"
#include "wizards/learning_wizard.h"


#include "./elements/element_factory.h"
#include "simulation/simulation.h"

namespace dnf_composer
{
	namespace user_interface
	{
		class SimulationWindow : public UserInterfaceWindow
		{
		private:
			std::shared_ptr<Simulation> simulation;
		public:
			SimulationWindow(const std::shared_ptr<Simulation>& simulation);

			SimulationWindow(const SimulationWindow&) = delete;
			SimulationWindow& operator=(const SimulationWindow&) = delete;
			SimulationWindow(SimulationWindow&&) = delete;
			SimulationWindow& operator=(SimulationWindow&&) = delete;

			void render() override;
			~SimulationWindow() override = default;
		private:
			void renderStartSimulationButton() const;
			void renderAddElement() const;
			void renderSetInteraction() const;
			void renderRemoveElement() const;
			void renderElementProperties(const std::pair<int, std::string>& elementId) const;
			void renderLogElementProperties() const;
			void renderExportElementComponents() const;

			// unfortunately, these functions need to be here like this
			void addElementNeuralField() const;
			void addElementGaussStimulus() const;
			void addElementFieldCoupling() const;
			void addElementGaussKernel() const;
			void addElementMexicanHatKernel() const;
			void addElementNormalNoise() const;
			void addElementGaussFieldCoupling() const;
		};
	}
}
