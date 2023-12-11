#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <deque>

#include "user_interface/logger_window.h"

namespace dnf_composer
{
	class ArchitectureFileHandler
	{
	private:
		std::string architectureFileLocation;
		std::string fieldCouplingFileLocation;

		std::deque<std::string> unparsedDynamicNeuralFieldParameters;
		std::deque<std::string> unparsedDynamicNeuralFieldCouplingsParameters;

		std::string dynamicNeuralFieldParametersToSave;
		std::string dynamicNeuralFieldCouplingsParametersToSave;

	public:
		ArchitectureFileHandler(const std::string& identifier);
		ArchitectureFileHandler(const ArchitectureFileHandler& other) = default;
		ArchitectureFileHandler(ArchitectureFileHandler&& other) noexcept = default;
		ArchitectureFileHandler& operator=(const ArchitectureFileHandler& other) = default;
		ArchitectureFileHandler& operator=(ArchitectureFileHandler&& other) noexcept = default;
		~ArchitectureFileHandler() = default;

		void readArchitectureFile();
		void saveArchitectureFile();

		std::deque<std::string> getUnparsedDynamicNeuralFieldParameters();
		std::deque<std::string> getUnparsedDynamicNeuralFieldCouplingsParameters();

		void setDynamicNeuralFieldParametersToSave(const std::string& parameters);
		void setDynamicNeuralFieldCouplingsParametersToSave(const std::string& parameters);

	private:
		void readDynamicNeuralFieldParameters();
		void readDynamicNeuralFieldCouplingsParameters();

		void saveDynamicNeuralFieldParameters() const;
		void saveDynamicNeuralFieldCouplingsParameters() const;
	};
}