#include "elements/element.h"

Element::Element()
{
	label = ElementLabel::UNITIALIZED;
	uniqueIdentifier = std::string();
	size = 0;
	components["output"] = {};
	components["input"] = {};
	inputs = {};
}

void Element::addInput(const std::shared_ptr<Element>& inputElement, const std::string& inputComponent)
{
	if (!inputElement)
		throw Exception(ErrorCode::ELEM_INPUT_IS_NULL);

	auto existingInput = inputs.find(inputElement);
	if (existingInput != inputs.end())
		throw Exception(ErrorCode::ELEM_INPUT_ALREADY_EXISTS);
	
	// check if input element has the same size as the element
	if(inputElement->getComponentPtr("output")->size() != this->getComponentPtr("input")->size())
		if (inputElement->getComponentPtr("output")->size() != this->getSize())
			throw Exception(ErrorCode::ELEM_INPUT_SIZE_MISMATCH, inputElement->getUniqueIdentifier());

	inputs[inputElement] = inputComponent;
}

void Element::removeInput(const std::string& inputElementId)
{
	for (auto& [key, value] : inputs)
	{
		if (key->uniqueIdentifier == inputElementId)
		{
			inputs.erase(key);
			return;
		}
	}
	//throw Exception(ErrorCode::ELEM_INPUT_NOT_FOUND, inputElementId);
}

bool Element::hasInput(const std::string& inputElementId, const std::string& inputComponent)
{
	for (auto& [key, value] : inputs)
	{
		if (key->uniqueIdentifier == inputElementId && value == inputComponent)
			return true;
	}
	return false;
}

void Element::updateInput()
{
	std::fill(components["input"].begin(), components["input"].end(), 0);

	for (const auto& input_pair : inputs) {
		auto inputElement = input_pair.first;
		auto inputElementComponent = input_pair.second;
		auto& inputElementComponents = inputElement->components;
		const auto& inputElementComponentValue = inputElementComponents.at(inputElementComponent);

		for (int i = 0; i < inputElementComponentValue.size(); i++)
		{
			components["input"][i] += inputElementComponentValue[i];
		}
	}
}

void Element::setUniqueIdentifier(const std::string& uniqueIdentifier)
{
	//this->uniqueIdentifier = uniqueIdentifier;

	// for now, element renaming can be pottentially damaging for the simulation
	throw Exception(ErrorCode::ELEM_RENAME_NOT_ALLOWED, uniqueIdentifier);
}

void Element::setSize(uint8_t size)
{
	//this->size = size;
	//components.at("output").resize(size);
	//components.at("input").resize(size);

	// for now, element resizing can be pottentially damaging for the simulation
	throw Exception(ErrorCode::ELEM_SIZE_NOT_ALLOWED, uniqueIdentifier);
}

int Element::getSize()
{
	return size;
}

std::string Element::getUniqueIdentifier() const
{
	return uniqueIdentifier;
}

ElementLabel Element::getLabel()
{
	return label;
}

std::vector<double> Element::getComponent(const std::string& componentName)
{
	for (int i = 0; i < components.size(); i++)
		if (components.find(componentName) != components.end())
			return components.at(componentName);
	throw Exception(ErrorCode::ELEM_COMP_NOT_FOUND, uniqueIdentifier, componentName);
}

std::vector<double>* Element::getComponentPtr(const std::string& componentName)
{
	for (int i = 0; i < components.size(); i++)
		if (components.find(componentName) != components.end())
			return &components.at(componentName);
	throw Exception(ErrorCode::ELEM_COMP_NOT_FOUND, uniqueIdentifier, componentName);
}

std::vector<std::shared_ptr<Element>> Element::getInputs()
{
	std::vector<std::shared_ptr<Element>> inputVec;
	for (const auto& [key, value] : inputs)
		inputVec.push_back(key);

	return inputVec;
}

Element::~Element()
{
	// nothing requires cleanup 
}