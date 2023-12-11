#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <memory>
#include <ranges>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <cassert>
#include <cmath>

#include "../exceptions/exception.h"
#include "../user_interface/logger_window.h"

namespace dnf_composer
{

	namespace element
	{
		enum ElementLabel : int
		{
			UNINITIALIZED,
			NEURAL_FIELD,
			GAUSS_STIMULUS,
			GAUSS_KERNEL,
			MEXICAN_HAT_KERNEL,
			NORMAL_NOISE,
			FIELD_COUPLING,
			GAUSS_FIELD_COUPLING
		};

		inline const std::map<ElementLabel, std::string> ElementLabelToString = {
			{NEURAL_FIELD, "neural field" },
			{GAUSS_STIMULUS, "gauss stimulus" },
			{GAUSS_FIELD_COUPLING, "gauss field coupling" },
			{FIELD_COUPLING, "field coupling" },
			{GAUSS_KERNEL, "gauss kernel" },
			{MEXICAN_HAT_KERNEL, "mexican hat kernel" },
			{NORMAL_NOISE, "normal noise" },
		};

		enum class ElementDegeneracyType
		{
			NONE = 0,

			NEURONS_DEACTIVATE,
			NEURONS_DEACTIVATE_PERCENTAGE,

			WEIGHTS_DEACTIVATE,
			WEIGHTS_DEACTIVATE_PERCENTAGE,
			WEIGHTS_RANDOMIZE,
			WEIGHTS_RANDOMIZE_PERCENTAGE,
			WEIGHTS_REDUCE,
			WEIGHTS_REDUCE_PERCENTAGE,
		};

		class Element
		{
		protected:
			static inline int uniqueIdentifierCounter = 0;
			int uniqueIdentifier;
			std::string uniqueName;
			ElementLabel label;
			int size;
			std::unordered_map<std::string, std::vector<double>> components;
			std::unordered_map<std::shared_ptr<Element>, std::string> inputs;
		public:
			Element();
			virtual void init() = 0;
			virtual void step(double t, double deltaT) = 0;
			virtual void close() = 0;

			void addInput(const std::shared_ptr<Element>& inputElement, const std::string& inputComponent = "output");
			void removeInput(const std::string& inputElementId);
			void removeInput(int uniqueId);
			bool hasInput(const std::string& inputElementName, const std::string& inputComponent);
			bool hasInput(int inputElementId, const std::string& inputComponent);
			void updateInput();

			static void setUniqueIdentifier(int uniqueIdentifier);
			void setSize(int size) const;

			int getSize() const;
			int getUniqueIdentifier() const;
			std::string getUniqueName() const;
			ElementLabel getLabel() const;
			std::vector<double> getComponent(const std::string& componentName);
			std::vector<double>* getComponentPtr(const std::string& componentName);
			std::vector < std::shared_ptr<Element>> getInputs();

			virtual void printParameters() = 0;

			virtual ~Element() = default;
		};
	}
}