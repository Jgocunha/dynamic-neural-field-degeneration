#include "degenerate_neural_field.h"

DegenerateNeuralField::DegenerateNeuralField(const dnf_composer::element::ElementCommonParameters& elementCommonParameters,
	const dnf_composer::element::NeuralFieldParameters& parameters)
	: NeuralField(elementCommonParameters, parameters)
{
	degeneracyType = experiment::degeneration::ElementDegeneracyType::NONE;
	degenerate = false;
	populateIndicesForDegeneration();
	degeneratedIndices.clear();
}

void DegenerateNeuralField::init()
{
	NeuralField::init();
	populateIndicesForDegeneration(); // probably wont work in the inducing degeneration experiment
	degenerate = false;
}

void DegenerateNeuralField::calculateActivation(const double& t, const double& deltaT)
{
	for (int i = 0; i < commonParameters.dimensionParameters.size; i++)
	{
		auto it = std::find(degeneratedIndices.begin(), degeneratedIndices.end(), i);
		if (it != degeneratedIndices.end()) // Skip the "killed" neurons
		{
			components["activation"][i] = 0;
		}
		else
		{
			components["activation"][i] = components["activation"][i] + deltaT / parameters.tau *
				(-components["activation"][i] + components["resting level"][i] + components["input"][i]);
		}
	}
}

void DegenerateNeuralField::step(double t, double deltaT)
{
	updateInput();
	calculateActivation(t, deltaT);
	if (degenerate)
		applyDegeneracy();
	calculateOutput();
	updateState();
}

void DegenerateNeuralField::startDegeneration()
{
	degenerate = true;
}

void DegenerateNeuralField::setNumNeuronsToDegenerate(const int& numNeuronsToDegenerate)
{
	this->numNeuronsToDegenerate = numNeuronsToDegenerate;
}

void DegenerateNeuralField::applyDegeneracy()
{
	switch (degeneracyType)
	{
	case experiment::degeneration::ElementDegeneracyType::NEURONS_DEACTIVATE:
		for (int i = 0; i < numNeuronsToDegenerate; i++)
			setRandomUniqueNeuronToZero();
		degenerate = false;
		break;
	case experiment::degeneration::ElementDegeneracyType::NONE:
	case experiment::degeneration::ElementDegeneracyType::WEIGHTS_DEACTIVATE:
	case experiment::degeneration::ElementDegeneracyType::WEIGHTS_RANDOMIZE:
	case experiment::degeneration::ElementDegeneracyType::WEIGHTS_REDUCE:
		break;
	}
}

void DegenerateNeuralField::setDegeneracyType(experiment::degeneration::ElementDegeneracyType degeneracyType)
{
	this->degeneracyType = degeneracyType;
}

experiment::degeneration::ElementDegeneracyType DegenerateNeuralField::getDegeneracyType() const
{
	return degeneracyType;
}

double DegenerateNeuralField::getCentroid()
{
	const std::vector<double> f_output = dnf_composer::tools::math::heaviside(components["activation"], 0.1);
	double centroid = 0.0;

	if (*std::ranges::max_element(f_output) > 0)
	{
		const bool isAtLimits = (f_output[0] > 0) || (f_output[commonParameters.dimensionParameters.size - 1] > 0);

		double sumActivation = 0.0;
		double sumWeightedPositions = 0.0;

		for (int i = 0; i < commonParameters.dimensionParameters.size; i++)
		{
			const double activation = f_output[i];

			sumActivation += activation;

			// Calculate the circular distance from the midpoint to the position
			double distance = 0.0;
			if (isAtLimits)
				distance = fmod(static_cast<double>(i) - static_cast<double>(commonParameters.dimensionParameters.size) * 0.5 
					+ static_cast<double>(commonParameters.dimensionParameters.size) * 10, static_cast<double>(commonParameters.dimensionParameters.size));
			else
				distance = fmod(static_cast<double>(i) - static_cast<double>(commonParameters.dimensionParameters.size) * 0.5, static_cast<double>(commonParameters.dimensionParameters.size));
			sumWeightedPositions += distance * activation;
		}

		static constexpr double epsilon = 1e-6;
		if (std::fabs(sumActivation) > epsilon)
		{
			// Shift the centroid back to the circular field
			centroid = fmod(static_cast<double>(commonParameters.dimensionParameters.size) * 0.5 + sumWeightedPositions / sumActivation, static_cast<double>(commonParameters.dimensionParameters.size));
			if (isAtLimits)
				centroid = (centroid >= 0 ? centroid : centroid + static_cast<double>(commonParameters.dimensionParameters.size));
		}
		centroid = centroid * commonParameters.dimensionParameters.d_x + commonParameters.dimensionParameters.d_x;
	}
	else
	{
		centroid = -1.0;
	}
	return centroid;
}


void DegenerateNeuralField::clearDegeneration()
{
	degeneratedIndices.clear();
}

void DegenerateNeuralField::populateIndicesForDegeneration()
{
	for (int i = 0; i < commonParameters.dimensionParameters.size; i++)
		indicesForDegeneration.push_back(i);
}

void DegenerateNeuralField::setRandomUniqueNeuronToZero()
{
	if (indicesForDegeneration.empty())
	{
		// No indices available for degeneration
		return;
	}

	// Randomly select an index from indicesForDegeneration
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, indicesForDegeneration.size() - 1);
	const int randomIndex = indicesForDegeneration[dis(gen)];

	// Push the random index to degeneratedIndices
	degeneratedIndices.push_back(randomIndex);

	// Remove the random index from indicesForDegeneration
	indicesForDegeneration.erase(std::remove(indicesForDegeneration.begin(), indicesForDegeneration.end(), randomIndex), indicesForDegeneration.end());
}

void DegenerateNeuralField::reset()
{
	indicesForDegeneration.clear();
	degeneratedIndices.clear();
	degenerate = false;
}
