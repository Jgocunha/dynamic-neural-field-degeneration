#pragma once

#include "mathtools/mathtools.h"

#include "element.h"

#include <set>

namespace dnf_composer
{
	enum class LearningRule
	{
		HEBBIAN = 0,
		DELTA_WIDROW_HOFF,
		DELTA_KROGH_HERTZ
	};

	namespace element
	{
		struct FieldCouplingParameters
		{
			int inputFieldSize;
			double scalar;
			double learningRate;
			LearningRule learningRule;
		};

		class FieldCoupling : public Element
		{
		protected:
			FieldCouplingParameters parameters;
			std::vector<std::vector<double>> weights;
			bool trained;
			bool updateAllWeights;
			std::string weightsFilePath;
		public:
			FieldCoupling(const std::string& id, int outputSize, const FieldCouplingParameters& parameters);

			void init() override;
			void step(double t, double deltaT) override;
			void close() override;

			void printParameters() override;

			void setWeightsFilePath(const std::string& filePath);
			bool readWeights();
			void resetWeights();
			void saveWeights() const;
			virtual void updateWeights(const std::vector<double>& input, const std::vector<double>& output);

			void setLearningRate(double learningRate);
			void setUpdateAllWeights(bool updateAllWeights);

			const std::vector<std::vector<double>>& getWeights() const;

			~FieldCoupling() override = default;

		protected:
			void getInputFunction();
			void computeOutput();
			void scaleOutput();

			void writeWeights() const;
		};
	}
}
