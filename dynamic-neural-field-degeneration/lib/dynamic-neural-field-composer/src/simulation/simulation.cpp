#include "simulation/simulation.h"


Simulation::Simulation(const double& deltaT, const double& tZero, const double& t)
	: deltaT(deltaT), tZero(tZero), t(t)
{
	initialized = false;
	elements = {};
}

void Simulation::init()
{
	t = tZero;
	for (int i = 0; i < elements.size(); i++)
		elements[i]->init();

	initialized = true;
}

void Simulation::step()
{
	t += deltaT;
	for (int i = 0; i < elements.size(); i++)
		elements[i]->step(t, deltaT);
}

void Simulation::close()
{
	for (int i = 0; i < elements.size(); i++)
		elements[i]->close();

	initialized = false;
}

void Simulation::run(const double& runTime)
{
	if (runTime <= 0)
		throw Exception(ErrorCode::SIM_RUNTIME_LESS_THAN_ZERO, (int)runTime);

	double simTime = t + runTime;

	if (!initialized)
		init();

	while (t < simTime)
		step();

	close();
}

void Simulation::addElement(const std::shared_ptr<Element>& element)
{
	// Check if an element with the same id already exists
	std::string newElementId = element->getUniqueIdentifier();
	for (int i = 0; i < elements.size(); i++) {
		if (elements[i]->getUniqueIdentifier() == newElementId) {
			throw Exception(ErrorCode::SIM_ELEM_ALREADY_EXISTS, newElementId);
			// Element with the same id already exists, throw an exception or handle the error
			//throw Exception(ErrorCode::SIM_ELEM_ALREADY_EXISTS, newElementId);
			// Alternatively, you can handle the error in any other way that makes sense for your application
			// such as logging an error message, returning an error code, etc.
			// ...
		}
	}

	elements.push_back(element);
	element->init();
}

void Simulation::removeElement(const std::string& elementId)
{
	for (int i = 0; i < elements.size(); i++)
	{
		// Remove the element from the inputs of all other elements
		elements[i]->removeInput(elementId);
	}

	for (int i = 0; i < elements.size(); i++)
	{
		// Remove the element from the simulation
		if (elements[i]->getUniqueIdentifier() == elementId)
		{
			elements.erase(elements.begin() + i);
			return;
		}
	}
	throw Exception(ErrorCode::SIM_ELEM_NOT_FOUND, elementId);
}

void Simulation::resetElement(const std::string& idOfElementToReset, const std::shared_ptr<Element>& newElement)
{
	bool elementFound = false;
	for (int i = 0; i < elements.size(); i++)
	{
		if (elements[i]->getUniqueIdentifier() == idOfElementToReset)
		{
			elements[i] = newElement;
			elements[i]->init();
			elementFound = true;
			break;
		}
	}

	if (!elementFound)
		throw Exception(ErrorCode::SIM_ELEM_NOT_FOUND, idOfElementToReset);

}

void Simulation::createInteraction(const std::string& stimulusElementId, 
	const std::string& stimulusComponent, const std::string& receivingElementId)
{
	std::shared_ptr<Element> stimulusElement = getElement(stimulusElementId);
	std::shared_ptr<Element> receivingElement = getElement(receivingElementId);
	receivingElement->addInput(stimulusElement, stimulusComponent);
}


std::shared_ptr<Element> Simulation::getElement(const std::string& id) const
{
	for (int i = 0; i < elements.size(); i++)
		if (elements[i]->getUniqueIdentifier() == id)
			return std::shared_ptr<Element>(elements[i]);

	throw Exception(ErrorCode::SIM_ELEM_NOT_FOUND, id);
}

std::shared_ptr<Element> Simulation::getElement(uint8_t index) const 
{
	if (index < elements.size())
		return elements[index];
	else
		throw Exception(ErrorCode::SIM_ELEM_INDEX, (int)index);
}

std::vector<double> Simulation::getComponent(const std::string& id, const std::string& componentName)
{

	std::shared_ptr<Element> foundElement = getElement(id);
	return foundElement->getComponent(componentName);
}

std::vector<double>* Simulation::getComponentPtr(const std::string& id, const std::string& componentName)
{
	std::shared_ptr<Element> foundElement = getElement(id);
	return foundElement->getComponentPtr(componentName);
}

uint8_t Simulation::getNumberOfElements() const
{
	return elements.size();
}

std::vector<std::shared_ptr<Element>> Simulation::getElementsThatHaveSpecifiedElementAsInput(const std::string& specifiedElement, const std::string& inputComponent)
{
	std::vector<std::shared_ptr<Element>> elementsThatHaveSpecifiedElementAsInput;
	for (int i = 0; i < elements.size(); i++)
	{
		if (elements[i]->hasInput(specifiedElement, inputComponent))
			elementsThatHaveSpecifiedElementAsInput.push_back(elements[i]);
	}
	return elementsThatHaveSpecifiedElementAsInput;
}

bool Simulation::isInitialized()
{
	return initialized;
}

Simulation::~Simulation()
{
	// nothing requires cleanup
}
