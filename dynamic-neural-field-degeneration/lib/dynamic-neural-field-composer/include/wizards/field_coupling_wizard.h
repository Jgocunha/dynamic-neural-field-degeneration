#pragma once

#include "./simulation/simulation.h"
#include "./elements/neural_field.h"
#include "./elements/field_coupling.h"
#include "./elements/gauss_stimulus.h"

class FieldCouplingWizard
{
private:
	std::shared_ptr<Simulation> simulation;
	std::shared_ptr<FieldCoupling> fieldCoupling;

	std::shared_ptr<NeuralField> neuralFieldPre;
	std::shared_ptr<NeuralField> neuralFieldPost;

	std::vector<std::vector<double>> targetPeakLocationsForNeuralFieldPre;
	std::vector<std::vector<double>> targetPeakLocationsForNeuralFieldPost;

	GaussStimulusParameters gaussStimulusParameters = {15, 3};

	std::string pathToFieldActivationPre;
	std::string pathToFieldActivationPost;

public:
	FieldCouplingWizard() = default;
	FieldCouplingWizard(const std::shared_ptr<Simulation> simulation, const std::string& fieldCouplingUniqueId);
	~FieldCouplingWizard() = default;

	void setGaussStimulusParameters(const GaussStimulusParameters& gaussStimulusParameters);
	void setTargetPeakLocationsForNeuralFieldPre(const std::vector<std::vector<double>>& targetPeakLocationsForNeuralFieldPre);
	void setTargetPeakLocationsForNeuralFieldPost(const std::vector<std::vector<double>>& targetPeakLocationsForNeuralFieldPost);

	void simulateAssociation();
	void trainWeights(const int iterations);

private:
	void setFieldCoupling(const std::string& fieldCouplingUniqueId);
	void setNeuralFieldPre();
	void setNeuralFieldPost();

	std::vector<double> normalizeFieldActivation(std::vector<double>& vec, const double& restingLevel);
	void saveFieldActivation(const std::vector<double>* fieldActivation, const std::string& filename);
	std::vector<double> readFieldActivation(const std::string& filename, const int line);
};