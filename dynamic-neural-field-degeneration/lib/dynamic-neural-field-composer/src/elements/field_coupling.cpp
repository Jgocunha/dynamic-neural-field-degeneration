// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "elements/field_coupling.h"

namespace dnf_composer
{
	namespace element
	{

		FieldCoupling::FieldCoupling(const std::string& id, int size, const FieldCouplingParameters& parameters)
			: parameters(parameters)
		{
			if (size <= 0)
				throw Exception(ErrorCode::ELEM_INVALID_SIZE, id);
			if (parameters.inputFieldSize <= 0)
				throw Exception(ErrorCode::ELEM_SIZE_NOT_ALLOWED, id);

			this->label = ElementLabel::FIELD_COUPLING;
			this->uniqueName = id;
			this->size = size;
			components["input"] = std::vector<double>(parameters.inputFieldSize);
			components["output"] = std::vector<double>(size);
			mathtools::resizeMatrix(weights, static_cast<int>(components["input"].size()), static_cast<int>(components["output"].size()));

			// Initialize the weight matrix with random values
			mathtools::fillMatrixWithRandomValues(weights, -1, 1);

			weightsFilePath = std::string(OUTPUT_DIRECTORY) + "/" + uniqueName + "_weights.txt";

			updateAllWeights = true;
			trained = false;
		}

		void FieldCoupling::init()
		{
			std::ranges::fill(components["input"], 0);
			std::ranges::fill(components["output"], 0);

			if (readWeights())
				trained = true;
			else
			{
				mathtools::resizeMatrix(weights, static_cast<int>(components["input"].size()), static_cast<int>(components["output"].size()));
				mathtools::fillMatrixWithRandomValues(weights, 0.0, 0.0);
				trained = false;
				writeWeights();
			}
		}

		void FieldCoupling::step(double t, double deltaT)
		{
			getInputFunction();
			computeOutput();
			scaleOutput();
		}

		void FieldCoupling::close()
		{
			std::ranges::fill(components["input"], 0);
			std::ranges::fill(components["output"], 0);
			resetWeights();
		}

		void FieldCoupling::printParameters()
		{
			std::ostringstream logStream;

			logStream << std::left;

			logStream << "Logging element parameters" << std::endl;
			logStream << "Unique Identifier: " << uniqueIdentifier << std::endl;
			logStream << "Unique Name: " << uniqueName << std::endl;
			logStream << "Label: " << ElementLabelToString.at(label) << std::endl;
			logStream << "Size: " << size << std::endl;

			logStream << "Components: ";
			for (const auto& pair : components)
			{
				const std::string& componentName = pair.first;
				const std::vector<double>& componentValues = pair.second;

				logStream << componentName << " | ";
			}

			logStream << std::endl << "Inputs: ";
			for (const auto& inputPair : inputs)
			{
				const std::shared_ptr<Element>& inputElement = inputPair.first;
				const std::string& inputComponent = inputPair.second;

				logStream << inputElement->getUniqueName() << "->" << inputComponent << " | ";
			}

			logStream << std::endl << "FieldCouplingParameters: ";
			logStream << "Input Field Size: " << parameters.inputFieldSize << " | ";
			logStream << "Scalar: " << parameters.scalar << " | ";
			logStream << "Learning Rate: " << parameters.learningRate << " | ";
			logStream << "Learning Rule: ";
			switch (parameters.learningRule)
			{
			case LearningRule::HEBBIAN:
				logStream << "hebbian rule";
				break;
			case LearningRule::DELTA_WIDROW_HOFF:
				logStream << "delta rule Widrow Hoff variation";
				break;
			case LearningRule::DELTA_KROGH_HERTZ:
				logStream << "delta rule Krogh and Hertz variation";
				break;
			}

			user_interface::LoggerWindow::addLog(user_interface::LogLevel::_INFO, logStream.str().c_str());
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

		void FieldCoupling::setUpdateAllWeights(bool updateAllWeights)
		{
			this->updateAllWeights = updateAllWeights;
		}

		void FieldCoupling::updateWeights(const std::vector<double>& input, const std::vector<double>& output)
		{
			switch (parameters.learningRule)
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

		void FieldCoupling::setLearningRate(double learningRate)
		{
			parameters.learningRate = learningRate;
		}

		const std::vector<std::vector<double>>& FieldCoupling::getWeights() const
		{
			return weights;
		}

		bool FieldCoupling::readWeights()
		{
			std::ifstream file(weightsFilePath); 

			if (file.is_open()) {
				mathtools::resizeMatrix(weights, 0, 0);
				double element;
				std::vector<double> row;
				while (file >> element) 
				{  
					row.push_back(element);  
					if (row.size() == components["output"].size())
					{
						weights.push_back(row);  
						row.clear(); 
					}
				}
				file.close();
				const std::string message = "Weights '" + this->getUniqueName() + "' read successfully from: " + weightsFilePath;
				user_interface::LoggerWindow::addLog(user_interface::LogLevel::_INFO, message.c_str());
				return true;
			}
			else
			{
				const std::string message = "Failed to read weights '" + this->getUniqueName() + "' from: " + weightsFilePath;
				user_interface::LoggerWindow::addLog(user_interface::LogLevel::_WARNING, message.c_str());
			}
			return false;
		}

		void FieldCoupling::writeWeights() const
		{
			std::ofstream file(weightsFilePath);

			if (file.is_open()) {
				for (const auto& row : weights) {
					for (const auto& element : row) {
						file << element << " ";  
					}
					file << '\n'; 
				}
				file.close();
				const std::string message = "Saved weights '" + this->getUniqueName() +"' to: " + weightsFilePath;
				user_interface::LoggerWindow::addLog(user_interface::LogLevel::_INFO, message.c_str());
			}
			else
			{
				const std::string message = "Failed to saved weights '" + this->getUniqueName() + "' to: " + weightsFilePath;
				user_interface::LoggerWindow::addLog(user_interface::LogLevel::_ERROR, message.c_str());
			}
		}

		void FieldCoupling::saveWeights() const
		{
			writeWeights();
		}

		void FieldCoupling::setWeightsFilePath(const std::string& filePath)
		{
			weightsFilePath = filePath + "/" + uniqueName + "_weights.txt";
		}
	}
}
