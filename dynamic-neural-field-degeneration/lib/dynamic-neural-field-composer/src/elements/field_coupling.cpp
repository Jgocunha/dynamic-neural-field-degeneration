#include "elements/field_coupling.h"

FieldCoupling::FieldCoupling(const std::string& id, const int& outputSize, const int& inputSize, const FieldCouplingParameters& parameters, const LearningRule& learningRule)
	: parameters(parameters), learningRule(learningRule)
{
	// Assert that the sizes are positive
	assert(outputSize > 0 && inputSize > 0);

	this->label = ElementLabel::FIELD_COUPLING;
	this->uniqueIdentifier = id;
	this->size = outputSize;
	components["input"] = std::vector<double>(inputSize); 
	components["output"] = std::vector<double>(outputSize);
	mathtools::resizeMatrix(weights, components["input"].size(), components["output"].size());

	// Initialize the weight matrix with random values
	mathtools::fillMatrixWithRandomValues(weights, -1, 1);

	weightsFilePath = std::string(OUTPUT_DIRECTORY) + "/" + uniqueIdentifier + "_weights.txt";
}


void FieldCoupling::init()
{
	std::fill(components["input"].begin(), components["input"].end(), 0);
	std::fill(components["output"].begin(), components["output"].end(), 0);

	if(readWeights())
		trained = true;
	else
	{
		mathtools::resizeMatrix(weights, components["input"].size(), components["output"].size());
		mathtools::fillMatrixWithRandomValues(weights, 0.0, 0.0);
		trained = false;
		writeWeights();
	}
}

void FieldCoupling::step(const double& t, const double& deltaT)
{
	getInputFunction();
	computeOutput();
	scaleOutput();
}

void FieldCoupling::close()
{
	std::fill(components["input"].begin(), components["input"].end(), 0);
	std::fill(components["output"].begin(), components["output"].end(), 0);
	// empty weight matrix
}

void FieldCoupling::getInputFunction()
{
	// get input
	updateInput();
	// only the positive values of the input are considered
	for (auto& value : components["input"])
		if (value < 0)
			value = 0;
}

void FieldCoupling::computeOutput()
{
	// multiply the input by the weights to get output
	for (int i = 0; i < components["output"].size(); i++)
		for (int j = 0; j < components["input"].size(); j++)
			components["output"][i] += weights[j][i] * components["input"][j];

	// only the positive values of the output are considered
	for (auto& value : components["output"])
		if (value < 0)
			value = 0;
}

void FieldCoupling::scaleOutput()
{
	// Scale the output by parameter k
	for (auto& value : components["output"])
		value *= parameters.scalar;
}

void FieldCoupling::resetWeights()
{
	// empty weight matrix
	mathtools::fillMatrixWithRandomValues(weights, 0, 0);
}

void FieldCoupling::setUpdateAllWeights(const bool& updateAllWeights)
{
		this->updateAllWeights = updateAllWeights;
}

void FieldCoupling::updateWeights(const std::vector<double>& input, const std::vector<double>& output)
{
	std::cout << "Updating weights" << std::endl;
	switch (learningRule)
	{
	case LearningRule::HEBBIAN:
		weights = mathtools::hebbLearningRule(input, output, parameters.learningRate);
		break;
	case LearningRule::DELTA_WIDROW_HOFF:
		weights = mathtools::deltaLearningRuleWidrowHoff(weights, input, output, parameters.learningRate);
		break;
	case LearningRule::DELTA_KROGH_HERTZ:
		weights = mathtools::deltaLearningRuleKroghHertz(weights, input, output, parameters.learningRate);
		break;
	}

	writeWeights();
}

void FieldCoupling::setLearningRate(const double& learningRate)
{
	parameters.learningRate = learningRate;
}

const std::vector<std::vector<double>>& FieldCoupling::getWeights() const
{
	return weights;
}

bool FieldCoupling::readWeights()
{
	std::ifstream file(weightsFilePath);  // Open file for reading

	if (file.is_open()) {
		mathtools::resizeMatrix(weights, 0, 0);
		double element;
		std::vector<double> row;
		while (file >> element) {  // Read each element from file
			row.push_back(element);  // Add element to the current row
			if (row.size() == components["output"].size())
			{
				weights.push_back(row);  // Add row to the vector of weights
				row.clear();  // Clear the row for the next iteration
			}
		}
		file.close();  // Close the file
		return true;
	}
	else
	{
		std::cout << "Failed to read from weights.txt" << std::endl;
	}
	return false;
}

void FieldCoupling::writeWeights() const
{
	std::ofstream file(weightsFilePath); // Open file for writing

	if (file.is_open()) {
		// Loop through each row of weights
		for (const auto& row : weights) {
			// Loop through each element in the row
			for (const auto& element : row) {
				file << element << " ";  // Write element to file separated by a space
			}
			file << '\n';  // Write new line after each row
		}
		file.close();  // Close the file
	}
	else
	{
		std::cout << "Failed to write to weights.txt" << std::endl;
	}
}

void FieldCoupling::saveWeights() const
{
	writeWeights();
}

void FieldCoupling::setWeightsFilePath(const std::string& filePath)
{
	weightsFilePath = filePath + "/" + uniqueIdentifier + "_weights.txt";
}
