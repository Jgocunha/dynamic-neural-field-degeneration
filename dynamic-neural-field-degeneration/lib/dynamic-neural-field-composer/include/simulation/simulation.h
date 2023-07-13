#pragma once

#include <vector>
#include <memory>
#include <string>
#include <filesystem>

#include "elements/element.h"
#include "exceptions/exception.h"


class Simulation
{
protected:
	bool initialized;
	std::vector<std::shared_ptr<Element>> elements;
	std::string uniqueIdentifier;
public:
	double deltaT;
	double tZero;
	double t;
public:
	Simulation(const double& deltaT = 1, const double& tZero = 0, const double& t = 0);

	void init();
	void step();
	void run(const double& runTime);
	void close();

	void addElement(const std::shared_ptr<Element>& element);
	void removeElement(const std::string& elementId);
	void resetElement(const std::string& idOfElementToReset, const std::shared_ptr<Element>& newElement);

	void createInteraction(const std::string& stimulusElementId, const std::string& stimulusComponent, 
		const std::string& receivingElementId);
	//void trainCoupling(const std::string& couplingUniqueIdentifier);

	std::shared_ptr<Element> getElement(const std::string& id) const;
	std::shared_ptr<Element> getElement(uint8_t index) const;
	std::vector<double> getComponent(const std::string& id, const std::string& componentName);
	std::vector<double>* getComponentPtr(const std::string& id, const std::string& componentName);
	uint8_t getNumberOfElements() const;
	std::vector < std::shared_ptr<Element>> getElementsThatHaveSpecifiedElementAsInput(const std::string& specifiedElement, 
		const std::string& inputComponent = "output");


	bool isInitialized();

	~Simulation();
};