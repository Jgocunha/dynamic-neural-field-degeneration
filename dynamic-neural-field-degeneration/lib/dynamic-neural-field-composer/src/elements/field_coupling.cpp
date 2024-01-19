// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "elements/field_coupling.h"


namespace dnf_composer
{
	namespace element
	{

		FieldCoupling::FieldCoupling(const ElementCommonParameters& elementCommonParameters, const FieldCouplingParameters& parameters)
			: Element(elementCommonParameters), parameters(parameters)
		{
			if (parameters.inputFieldSize <= 0)
				throw Exception(ErrorCode::ELEM_SIZE_NOT_ALLOWED, commonParameters.identifiers.uniqueName);

			commonParameters.identifiers.label = ElementLabel::FIELD_COUPLING;

			components["input"] = std::vector<double>(parameters.inputFieldSize);
			utilities::resizeMatrix(weights, static_cast<int>(components["input"].size()), static_cast<int>(components["output"].size()));

			// Initialize the weight matrix with random values
			utilities::fillMatrixWithRandomValues(weights, -1, 1);

			weightsFilePath = std::string(OUTPUT_DIRECTORY) + "/" + commonParameters.identifiers.uniqueName + "_weights.txt";

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
				utilities::resizeMatrix(weights, static_cast<int>(components["input"].size()), static_cast<int>(components["output"].size()));
				utilities::fillMatrixWithRandomValues(weights, 0.0, 0.0);
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
			printCommonParameters();

			std::ostringstream logStream;

			logStream << "Logging specific element parameters" << std::endl;
			logStream << "Input Field Size: " << parameters.inputFieldSize << std::endl;
			logStream << "Scalar: " << parameters.scalar << std::endl;
			logStream << "Learning Rate: " << parameters.learningRate << std::endl;
			logStream << "Learning Rule: ";
			switch (parameters.learningRule)
			{
			case LearningRule::HEBBIAN:
				logStream << "Hebbian learning rule" << std::endl;;
				break;
			case LearningRule::DELTA_WIDROW_HOFF:
				logStream << "Delta learning rule Widrow Hoff variation" << std::endl;;
				break;
			case LearningRule::DELTA_KROGH_HERTZ:
				logStream << "Delta learning rule Krogh and Hertz variation" << std::endl;;
				break;
			}

			log(LogLevel::INFO, logStream.str());
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
			utilities::fillMatrixWithRandomValues(weights, 0, 0);
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
		}

		void FieldCoupling::setLearningRate(double learningRate)
		{
			parameters.learningRate = learningRate;
		}

		const std::vector<std::vector<double>>& FieldCoupling::getWeights() const
		{
			return weights;
		}
#

		bool FieldCoupling::readWeights()
		{
			std::ifstream file(weightsFilePath);

			log(LogLevel::DEBUG, "Attempting to read weights...\n");

			const int originalMatrixRows = static_cast<int>(weights.size());
			const int originalMatrixColumns = static_cast<int>( weights[0].size());


			if (file.is_open()) 
			{
				utilities::resizeMatrix(weights, 0, 0);
				double element;
				std::vector<double> row;

				while (file >> element) {
					row.push_back(element);
					if (row.size() == components["output"].size()) {
						weights.push_back(row);
						row.clear();
					}
				}

				file.sync();
				int newMatrixRows = 0;
				int newMatrixColumns = 0;

				// Check if the dimensions of the loaded matrix match the original matrix
				do
				{
					newMatrixRows = static_cast<int>(weights.size());
					newMatrixColumns = static_cast<int>(weights[0].size());
					log(DEBUG, "Original matrix size is: " + std::to_string(originalMatrixRows) + " by " + std::to_string(originalMatrixColumns)
						+ ". Current matrix size is: " + std::to_string(newMatrixRows) + " by " + std::to_string(newMatrixColumns) + ".\n");
				} while (!(newMatrixRows == originalMatrixRows && !weights.empty() &&
					newMatrixColumns == originalMatrixColumns));

				const std::string message = "Weights '" + this->getUniqueName() +
					"' read successfully from: " + weightsFilePath + ". \n";
				log(LogLevel::INFO, message);

				file.close();

				return true;
			}

			const std::string message = "Failed to open file for reading weights '" + this->getUniqueName() +
				"' from: " + weightsFilePath + ". \n";
			log(LogLevel::ERROR_, message);

			return false;
		}



		void FieldCoupling::writeWeights() const
		{
			std::ofstream file(weightsFilePath);

			log(LogLevel::DEBUG, "Attempting to write weights...\n");

			if (file.is_open()) {
				for (const auto& row : weights) {
					for (const auto& element : row) {
						file << element << " ";
					}
					file << '\n';
				}

				// Flush the data to the file
				file.flush();

				if (file.good()) {
					file.close();
					while (file.is_open());
					const std::string message = "Saved weights '" + this->getUniqueName() + "' to: " + weightsFilePath + ". \n";
					log(LogLevel::INFO, message);
				}
				else {
					const std::string message = "Failed to save weights '" + this->getUniqueName() + "' to: " + weightsFilePath + ". \n";
					log(LogLevel::ERROR_, message);
				}
			}
			else {
				const std::string message = "Failed to open file for saving weights '" + this->getUniqueName() + "' to: " + weightsFilePath + ". \n";
				log(LogLevel::ERROR_, message);
			}
		}

		void FieldCoupling::saveWeights() const
		{
			writeWeights();
		}

		void FieldCoupling::setWeightsFilePath(const std::string& filePath)
		{
			weightsFilePath = filePath + "/" + commonParameters.identifiers.uniqueName + "_weights.txt";
		}
	}
}
