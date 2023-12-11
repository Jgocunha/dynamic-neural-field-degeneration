#pragma once

#include <vector>
#include <memory>
#include <string>
#include <filesystem>

#include "user_interface/logger_window.h"
#include "elements/element.h"
#include "exceptions/exception.h"

namespace dnf_composer
{
	class Simulation
	{
	protected:
		bool initialized;
		std::vector<std::shared_ptr<element::Element>> elements;
		std::string uniqueIdentifier;
	public:
		double deltaT;
		double tZero;
		double t;
	public:
		Simulation(double deltaT = 1, double tZero = 0, double t = 0);
		Simulation(const Simulation&) = delete;
		Simulation& operator=(const Simulation&) = delete;
		Simulation(Simulation&&) = delete;
		Simulation& operator=(Simulation&&) = delete;

		void init();
		void step();
		void run(double runTime);
		void close();

		void addElement(const std::shared_ptr<element::Element>& element);
		void removeElement(const std::string& elementId);
		void resetElement(const std::string& idOfElementToReset, const std::shared_ptr<element::Element>& newElement);

		void createInteraction(const std::string& stimulusElementId, const std::string& stimulusComponent, 
			const std::string& receivingElementId) const;

		std::shared_ptr<element::Element> getElement(const std::string& id) const;
		std::shared_ptr<element::Element> getElement(int index) const;
		std::vector<double> getComponent(const std::string& id, const std::string& componentName) const;
		std::vector<double>* getComponentPtr(const std::string& id, const std::string& componentName) const;
		int getNumberOfElements() const;
		std::vector < std::shared_ptr<element::Element>> getElementsThatHaveSpecifiedElementAsInput(const std::string& specifiedElement, 
		                                                                                            const std::string& inputComponent = "output") const;

		bool isInitialized() const;

		~Simulation() = default;
	};
}
