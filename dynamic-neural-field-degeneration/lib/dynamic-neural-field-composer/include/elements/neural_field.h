#pragma once

#include "element.h"
#include "activation_function.h"

namespace dnf_composer
{
	namespace element
	{
		struct NeuralFieldParameters
		{
		    double tau;
		    double startingRestingLevel;
			std::unique_ptr<ActivationFunction> activationFunction;

			NeuralFieldParameters& operator=(const NeuralFieldParameters& other)
			{
				if (this != &other) // Check for self-assignment
				{
					tau = other.tau;
					startingRestingLevel = other.startingRestingLevel;

					// Ensure that activationFunction is properly cloned
					if (other.activationFunction)
						activationFunction = other.activationFunction->clone();
					else
						activationFunction.reset(); // Reset to nullptr if other has a nullptr
				}
				return *this;
			}

			NeuralFieldParameters()
				:tau(0), startingRestingLevel(0), activationFunction(nullptr)
			{}

			NeuralFieldParameters(double tau, double restingLevel, const ActivationFunction& activationFunction)
				: tau(tau), startingRestingLevel(restingLevel), activationFunction(activationFunction.clone())
			{ }

			NeuralFieldParameters(const NeuralFieldParameters& other)
			{
				tau = other.tau;
				startingRestingLevel = other.startingRestingLevel;
				if (other.activationFunction == nullptr)
					activationFunction = std::make_unique<HeavisideFunction>(0);
				else
					activationFunction = other.activationFunction->clone();
			}
		};

		class NeuralField : public Element
		{
		protected:
			NeuralFieldParameters parameters;
		    double centroid;
		public:
			NeuralField(const ElementCommonParameters& elementCommonParameters, const NeuralFieldParameters& parameters);

			void init() override;
			void step(double t, double deltaT) override;
			void close() override;

			void printParameters() override;

			void setParameters(const NeuralFieldParameters& parameters);
			NeuralFieldParameters getParameters() const;
		    double getCentroid() const;

			~NeuralField() override = default;

		protected:
			void calculateActivation(double t, double deltaT);
			void calculateOutput();
			void calculateCentroid();
		};
	}
}
