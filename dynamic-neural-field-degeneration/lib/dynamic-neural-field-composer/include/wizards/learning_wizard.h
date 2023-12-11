#pragma once

#include "./simulation/simulation.h"
#include "./elements/neural_field.h"
#include "./elements/field_coupling.h"
#include "./elements/gauss_stimulus.h"


namespace dnf_composer
{
	class LearningWizard
	{
	private:
		std::shared_ptr<Simulation> simulation;
		std::shared_ptr<element::FieldCoupling> fieldCoupling;

		std::shared_ptr<element::NeuralField> neuralFieldPre;
		std::shared_ptr<element::NeuralField> neuralFieldPost;

		std::vector<std::vector<double>> targetPeakLocationsForNeuralFieldPre;
		std::vector<std::vector<double>> targetPeakLocationsForNeuralFieldPost;

		element::GaussStimulusParameters gaussStimulusParameters = { 15, 3 };

		std::string pathToFieldActivationPre;
		std::string pathToFieldActivationPost;

	public:
		LearningWizard() = default;
		LearningWizard(const std::shared_ptr<Simulation>& simulation, const std::string& fieldCouplingUniqueId);
		~LearningWizard() = default;

		void setDataFilePath(const std::string& filePath);

		void setGaussStimulusParameters(const element::GaussStimulusParameters& gaussStimulusParameters);
		void setTargetPeakLocationsForNeuralFieldPre(const std::vector<std::vector<double>>& targetPeakLocationsForNeuralFieldPre);
		void setTargetPeakLocationsForNeuralFieldPost(const std::vector<std::vector<double>>& targetPeakLocationsForNeuralFieldPost);

		void simulateAssociation();
		void trainWeights(int iterations) const;
		void saveWeights() const;
		void clearTargetPeakLocationsFromFiles() const;

	private:
		void setFieldCoupling(const std::string& fieldCouplingUniqueId);
		void setNeuralFieldPre();
		void setNeuralFieldPost();

		static std::vector<double> readFieldActivation(const std::string& filename, int line);
		static void saveFieldActivation(const std::vector<double>* fieldActivation, const std::string& filename);
		static std::vector<double> normalizeFieldActivation(std::vector<double>& vec, const double& restingLevel);
	};
}
