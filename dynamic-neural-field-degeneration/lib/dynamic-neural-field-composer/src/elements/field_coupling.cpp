#include "elements/field_coupling.h"

FieldCoupling::FieldCoupling(const std::string& id, const uint8_t& size, const FieldCouplingParameters& parameters)
	: parameters(parameters)
{
	this->label = ElementLabel::FIELD_COUPLING;
	this->uniqueIdentifier = id;
	this->size = size;
	components["input"] = std::vector<double>(size);
	components["output"] = std::vector<double>(size);
	mathtools::resizeMatrix(weights, size, size);
}

void FieldCoupling::init()
{
	std::fill(components["input"].begin(), components["input"].end(), 0);
	std::fill(components["output"].begin(), components["output"].end(), 0);

	if(readWeights())
		trained = true;
	else
	{
		mathtools::resizeMatrix(weights, size, size);
		mathtools::fillMatrixWithRandomValues(weights, -1, 1);
		trained = false;
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

void FieldCoupling::trainWeights(const std::string& inputFilename, const std::string& outputFilename, const uint16_t& iterations)
{
	// check if the filenames, throw exceptions
	// check how much lines "temp_input.txt", and "temp_output.txt" have
	uint8_t numLinesInput = mathtools::countNumOfLinesInFile("temp_input.txt");
	uint8_t numLinesOutput = mathtools::countNumOfLinesInFile("temp_output.txt");
	if (numLinesInput != numLinesOutput)
		std::cerr << "The files " << inputFilename << " and " << outputFilename << " have a different number of lines.\n";

	// read data and update weights
	int lineCount = 0;
	std::vector<double> input = std::vector<double>(size);
	std::vector<double> output = std::vector<double>(size);
	for (int i = 0; i < iterations; i++)
	{
		input = readInputOrOutput(inputFilename, lineCount);
		output = readInputOrOutput(outputFilename, lineCount);
		updateWeights(input, output);
		lineCount = (lineCount + 1) % (numLinesInput);
	}
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
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			components["output"][i] += weights[j][i] * components["input"][j];
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

std::vector<double> FieldCoupling::readInputOrOutput(const std::string& filename, const uint8_t& line)
{
	std::ifstream file(filename);
	std::vector<double> data;

	if (file.is_open())
	{
		std::string lineData;
		uint8_t currentLine = 0;

		// Read lines from the file until the desired line is reached
		while (std::getline(file, lineData) && currentLine < line)
			currentLine++;

		if (currentLine == line)
		{
			std::istringstream iss(lineData);
			double value;
			while (iss >> value)
				data.push_back(value);
		}
		else
			std::cout << "Line " << static_cast<int>(line) << " not found in " << filename << std::endl;
		file.close();
	}
	else
		std::cout << "Failed to open file " << filename << std::endl;

	return data;
}

void FieldCoupling::writeInputOrOutput(const std::string& filename, const std::vector<double>* data)
{
	std::ofstream file(filename, std::ios::app); // Open file in append mode
	if (file.is_open())
	{
		for (const auto& element : (*data))
			file << element << " ";  // Write element to file separated by a space
		file << '\n';
		file.close();
		std::cout << "Data saved to " << filename << std::endl;
	}
	else
	{
		std::cout << "Failed to save data to " << filename << std::endl;
	}
}

void FieldCoupling::updateWeights(const std::vector<double> input, const std::vector<double> output)
{
	weights = mathtools::deltaLearningRule(weights, input, output, parameters.learningRate);
	writeWeights();
}

bool FieldCoupling::readWeights()
{
	std::ifstream file("weights.txt");  // Open file for reading
	if (file.is_open()) {
		mathtools::resizeMatrix(weights, 0, 0);
		double element;
		std::vector<double> row;
		while (file >> element) {  // Read each element from file
			row.push_back(element);  // Add element to the current row
			if (row.size() == size)
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

void FieldCoupling::writeWeights()
{
	std::ofstream file("weights.txt"); // Open file for writing
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

