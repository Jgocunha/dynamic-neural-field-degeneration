////// This is a personal academic project. Dear PVS-Studio, please check it.
////
////// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
////
//#include "architecture/architecture_builder.h"
//
//
//namespace dnf_composer
//{
//    ArchitectureBuilder::ArchitectureBuilder(std::string identifier, const std::shared_ptr<Simulation>& simulation)
//        : identifier(std::move(identifier)), simulation(simulation), fileHandler(ArchitectureFileHandler(this->identifier))
//    {
//        architectureParameters = element::CompoundElementParameters{};
//        dynamicNeuralField = DynamicNeuralField{};
//        dynamicNeuralFieldCoupling = DynamicNeuralFieldCoupling{};
//    }
//
//    void ArchitectureBuilder::readArchitecture()
//    {
//        fileHandler.readArchitectureFile();
//
//        std::deque<std::string> unparsedDynamicNeuralFieldParameters = fileHandler.getUnparsedDynamicNeuralFieldParameters();
//        std::deque<std::string> unparsedDynamicNeuralFieldCouplingsParameters = fileHandler.getUnparsedDynamicNeuralFieldCouplingsParameters();
//
//        const int numberOfFields = static_cast<int>(unparsedDynamicNeuralFieldParameters.size());
//
//        for (int i = 0; i < numberOfFields; i++)
//        {
//            const std::string line = unparsedDynamicNeuralFieldParameters.front();
//            unparsedDynamicNeuralFieldParameters.pop_front();  // Use pop_front to remove the front element
//
//            parseDynamicNeuralFieldParameters(line);
//            createDynamicNeuralFieldElements();
//            addElementsToSimulation();
//            setupInteractionsBetweenElements();
//            clearDynamicNeuralFieldParametersAndDynamicNeuralField();
//        }
//
//        const int numberOfCouplings = static_cast<int>(unparsedDynamicNeuralFieldCouplingsParameters.size());
//
//        for (int i = 0; i < numberOfCouplings; i++)
//        {
//            const std::string line = unparsedDynamicNeuralFieldCouplingsParameters.front();
//            unparsedDynamicNeuralFieldCouplingsParameters.pop_front();  // Use pop_front to remove the front element
//
//            parseDynamicNeuralFieldCouplingsParameters(line);
//            createDynamicNeuralFieldCouplings();
//            addDynamicNeuralFieldCouplingsToSimulation();
//            setupInteractionBetweenFields();
//            clearDynamicNeuralFieldCouplingsParameters();
//        }
//
//        user_interface::LoggerWindow::addLog(user_interface::LogLevel::_INFO, "Finished reading architecture parameters.");
//    }
//
//    void ArchitectureBuilder::saveArchitecture()
//    {
//        std::ostringstream dynamicNeuralFieldParameters;
//        std::ostringstream dynamicNeuralFieldCouplingParameters;
//
//        for (int i = 0; i < simulation->getNumberOfElements(); i++)
//        {
//            const auto element = simulation->getElement(i);
//
//            //std::cout << "Element id: " << element->getUniqueIdentifier() << " Element name: " << element->getUniqueName() << std::endl;
//
//            const auto elementLabel = element->getLabel();
//
//            switch (elementLabel)
//            {
//            case element::ElementLabel::NEURAL_FIELD:
//                dynamicNeuralField.neuralField = std::dynamic_pointer_cast<element::NeuralField>(element);
//                architectureParameters.nfp = dynamicNeuralField.neuralField->getParameters();
//
//                dynamicNeuralFieldParameters << dynamicNeuralField.neuralField->getMaxSpatialDimension() << " "
//                    << dynamicNeuralField.neuralField->getStepSize() << " " 
//                    << architectureParameters.nfp.tau << " "
//                    << -architectureParameters.nfp.startingRestingLevel << " "
//                    << dynamicNeuralField.activationFunctionType << " ";
//                if (dynamicNeuralField.activationFunctionType == element::ActivationFunctionType::SIGMOID)
//                    dynamicNeuralFieldParameters << architectureParameters.nfp.activationFunction..steepness << " ";
//                dynamicNeuralFieldParameters << architectureParameters.nfp.activationFunctionParameters.xShift << " ";
//
//                break;
//            case element::ElementLabel::GAUSS_KERNEL:
//                if (element->getUniqueName().find("noise kernel") == std::string::npos)
//                {
//                    dynamicNeuralField.kernel = std::dynamic_pointer_cast<element::GaussKernel>(element);
//                    architectureParameters.gkp = std::dynamic_pointer_cast<element::GaussKernel>(dynamicNeuralField.kernel)->getParameters();
//
//                    dynamicNeuralFieldParameters << element::ElementLabel::GAUSS_KERNEL << " "
//                        << architectureParameters.gkp.amplitude << " "
//                        << architectureParameters.gkp.sigma << " "
//                        << -architectureParameters.gkp.amplitudeGlobal << " ";
//                }
//                break;
//            case element::ElementLabel::MEXICAN_HAT_KERNEL:
//                dynamicNeuralField.kernel = std::dynamic_pointer_cast<element::MexicanHatKernel>(element);
//                architectureParameters.mhkp = std::dynamic_pointer_cast<element::MexicanHatKernel>(dynamicNeuralField.kernel)->getParameters();
//
//                dynamicNeuralFieldParameters << element::ElementLabel::MEXICAN_HAT_KERNEL << " "
//                    << architectureParameters.mhkp.amplitudeExc << " "
//                    << architectureParameters.mhkp.sigmaExc << " "
//                    << architectureParameters.mhkp.amplitudeInh << " "
//                    << architectureParameters.mhkp.sigmaInh << " "
//                    << -architectureParameters.mhkp.amplitudeGlobal << " ";
//                break;
//            case element::ElementLabel::NORMAL_NOISE:
//                dynamicNeuralField.normalNoise = std::dynamic_pointer_cast<element::NormalNoise>(element);
//                architectureParameters.nnp = dynamicNeuralField.normalNoise->getParameters();
//                dynamicNeuralFieldParameters << architectureParameters.nnp.amplitude << '\n';
//                break;
//            case element::ElementLabel::GAUSS_FIELD_COUPLING:
//            {
//                dynamicNeuralFieldCoupling.gaussianFieldCoupling = std::dynamic_pointer_cast<element::GaussFieldCoupling>(element);
//                architectureParameters.gfcp = dynamicNeuralFieldCoupling.gaussianFieldCoupling->getParameters();
//
//                const std::shared_ptr<element::Element> inputField = dynamicNeuralFieldCoupling.gaussianFieldCoupling->getInputs()[0];
//                const std::shared_ptr<element::Element> outputField = simulation->getElementsThatHaveSpecifiedElementAsInput(element->getUniqueName())[0];
//
//                dynamicNeuralFieldCouplingParameters << inputField->getUniqueIdentifier() << " "
//                    << outputField->getUniqueIdentifier() << " "
//                    << architectureParameters.gfcp.sigma << " ";
//
//                const int numberOfCouplings = static_cast<int>(dynamicNeuralFieldCoupling.gaussianFieldCoupling->getParameters().couplings.size());
//
//                dynamicNeuralFieldCouplingParameters << numberOfCouplings << " ";
//
//                for (const auto& coupling : dynamicNeuralFieldCoupling.gaussianFieldCoupling->getParameters().couplings)
//                    dynamicNeuralFieldCouplingParameters << coupling.x_i << " "
//                    << coupling.x_j << " "
//                    << coupling.w_i_j << " ";
//
//                dynamicNeuralFieldCouplingParameters << '\n';
//            }
//            break;
//            case element::ElementLabel::GAUSS_STIMULUS:
//            case element::ElementLabel::FIELD_COUPLING:
//            case element::ElementLabel::UNINITIALIZED:
//                // do nothing
//                break;
//            }
//
//        }
//
//        fileHandler.setDynamicNeuralFieldParametersToSave(dynamicNeuralFieldParameters.str());
//        fileHandler.setDynamicNeuralFieldCouplingsParametersToSave(dynamicNeuralFieldCouplingParameters.str());
//
//        fileHandler.saveArchitectureFile();
//
//        user_interface::LoggerWindow::addLog(user_interface::LogLevel::_INFO, "Finished saving architecture parameters.");
//    }
//
//    void ArchitectureBuilder::setArchitectureParameters(const element::CompoundElementParameters& elementParameters)
//    {
//        architectureParameters = elementParameters;
//    }
//
//    void ArchitectureBuilder::setDynamicNeuralField(const DynamicNeuralField& dynamicNeuralField)
//    {
//        this->dynamicNeuralField = dynamicNeuralField;
//    }
//
//    void ArchitectureBuilder::setDynamicNeuralFieldCoupling(const DynamicNeuralFieldCoupling& coupling)
//    {
//        dynamicNeuralFieldCoupling = coupling;
//    }
//
//    element::CompoundElementParameters ArchitectureBuilder::getArchitectureParameters() const
//    {
//        return architectureParameters;
//    }
//
//    DynamicNeuralField ArchitectureBuilder::getDynamicNeuralField() const
//    {
//        return dynamicNeuralField;
//    }
//
//    DynamicNeuralFieldCoupling ArchitectureBuilder::getDynamicNeuralFieldCoupling() const
//    {
//        return dynamicNeuralFieldCoupling;
//    }
//
//    std::string ArchitectureBuilder::getIdentifier() const
//    {
//        return identifier;
//    }
//
//    void ArchitectureBuilder::parseDynamicNeuralFieldParameters(const std::string& line)
//    {
//        // Format: "size dx tau h activationFunctionType [steepness xShift] kernelType [c sigma c_glob] q"
//        // Format: "size dx tau h activationFunctionType [xShift] kernelType [c_exc sigma_exc c_inh sigma_inh c_glob] q"
//
//        std::istringstream iss(line);
//
//        iss >> dynamicNeuralField.spatialDimensionParameters.x_max;
//
//        std::vector<double> parsedParameters;
//        double value;
//        // Parse the first 3 values as neural field parameters
//        for (int i = 0; i < 3 && iss >> value; ++i)
//            parsedParameters.push_back(value);
//
//        dynamicNeuralField.spatialDimensionParameters.d_x = parsedParameters[0];
//        architectureParameters.nfp.tau = parsedParameters[1];
//        architectureParameters.nfp.startingRestingLevel = -parsedParameters[2];
//
//        // Parse the activation function type
//        int tempAfpType;
//        iss >> tempAfpType;
//        architectureParameters.nfp.activationFunctionParameters.type = static_cast<element::ActivationFunctionType>(tempAfpType);
//        // Depending on the activation function, parse additional parameters
//        if (architectureParameters.nfp.activationFunctionParameters.type == element::ActivationFunctionType::Sigmoid)
//        {
//            iss >> architectureParameters.nfp.activationFunctionParameters.steepness;
//        }
//        iss >> architectureParameters.nfp.activationFunctionParameters.xShift;
//
//        // Parse the kernel type
//        iss >> dynamicNeuralField.kernelType;
//        // Depending on the kernel type, parse additional parameters
//        if (dynamicNeuralField.kernelType == element::ElementLabel::GAUSS_KERNEL) // Gaussian kernel
//        {
//            iss >> architectureParameters.gkp.amplitude
//                >> architectureParameters.gkp.sigma
//                >> architectureParameters.gkp.amplitudeGlobal;
//            architectureParameters.gkp.amplitudeGlobal = -architectureParameters.gkp.amplitudeGlobal;
//        }
//        else if (dynamicNeuralField.kernelType == element::ElementLabel::MEXICAN_HAT_KERNEL) // Mexican hat kernel
//        {
//            iss >> architectureParameters.mhkp.amplitudeExc
//                >> architectureParameters.mhkp.sigmaExc
//                >> architectureParameters.mhkp.amplitudeInh
//                >> architectureParameters.mhkp.sigmaInh
//                >> architectureParameters.mhkp.amplitudeGlobal;
//            architectureParameters.mhkp.amplitudeGlobal = -architectureParameters.mhkp.amplitudeGlobal;
//        }
//
//        // Parse the last value as the amplitude of the normal noise
//        iss >> architectureParameters.nnp.amplitude;
//    }
//
//    void ArchitectureBuilder::createDynamicNeuralFieldElements()
//    {
//        static int elementCount = 0;
//        dynamicNeuralField.neuralField = std::make_shared<element::NeuralField>({ "neural field " + std::to_string(elementCount++), dynamicNeuralField.spatialDimensionParameters }, architectureParameters.nfp);
//
//        if (dynamicNeuralField.kernelType == element::ElementLabel::GAUSS_KERNEL)
//            dynamicNeuralField.kernel = std::make_shared<element::GaussKernel>({ "gauss kernel " + std::to_string(elementCount++), dynamicNeuralField.spatialDimensionParameters }, architectureParameters.gkp);
//
//        if (dynamicNeuralField.kernelType == element::ElementLabel::MEXICAN_HAT_KERNEL)
//            dynamicNeuralField.kernel = std::make_shared<element::MexicanHatKernel>({ "mex-hat kernel " + std::to_string(elementCount++), dynamicNeuralField.spatialDimensionParameters }, architectureParameters.mhkp);
//
//        dynamicNeuralField.normalNoise = std::make_shared<element::NormalNoise>({ "normal noise " + std::to_string(elementCount++), dynamicNeuralField.spatialDimensionParameters },  architectureParameters.nnp);
//        element::GaussKernelParameters gkp;
//        gkp.amplitude = 0.2;
//        gkp.sigma = 0.25;
//        gkp.amplitudeGlobal = 0.0;
//        dynamicNeuralField.normalNoiseKernel = std::make_shared<element::GaussKernel>({"noise kernel " + std::to_string(elementCount++), dynamicNeuralField.spatialDimensionParameters}, gkp);
//    }
//
//    void ArchitectureBuilder::addElementsToSimulation() const
//    {
//        simulation->addElement(dynamicNeuralField.neuralField);
//        simulation->addElement(dynamicNeuralField.kernel);
//        simulation->addElement(dynamicNeuralField.normalNoise);
//        simulation->addElement(dynamicNeuralField.normalNoiseKernel);
//    }
//
//    void ArchitectureBuilder::setupInteractionsBetweenElements() const
//    {
//        dynamicNeuralField.neuralField->addInput(dynamicNeuralField.kernel);
//        dynamicNeuralField.kernel->addInput(dynamicNeuralField.neuralField);
//
//        dynamicNeuralField.neuralField->addInput(dynamicNeuralField.normalNoiseKernel);
//        dynamicNeuralField.normalNoiseKernel->addInput(dynamicNeuralField.normalNoise);
//    }
//
//    void ArchitectureBuilder::clearDynamicNeuralFieldParametersAndDynamicNeuralField()
//    {
//        dynamicNeuralField = DynamicNeuralField{};
//    }
//
//    void ArchitectureBuilder::parseDynamicNeuralFieldCouplingsParameters(const std::string& line)
//    {
//        std::istringstream iss(line);
//
//        // Format: "fieldId1 fieldId2 sigma numberOfCouplings(n) x_i1 x_j1 w_i_j1 x_i2 x_j2 w_i_j2 ... x_in x_jn w_i_jn"
//
//        iss >> dynamicNeuralFieldCoupling.idsOfCoupledDynamicNeuralFields[0]
//            >> dynamicNeuralFieldCoupling.idsOfCoupledDynamicNeuralFields[1]
//            >> architectureParameters.gfcp.sigma;
//
//        int numberOfCouplings;
//        iss >> numberOfCouplings;
//
//        for (int i = 0; i < numberOfCouplings; i++)
//        {
//            WeightedCoupling coupling = {};
//            iss >> coupling.x_i
//                >> coupling.x_j
//                >> coupling.w_i_j;
//            dynamicNeuralFieldCoupling.couplings.push_back(coupling);
//        }
//    }
//
//    void ArchitectureBuilder::createDynamicNeuralFieldCouplings()
//    {
//        static int elementCount = 0;
//
//        architectureParameters.gfcp.inputFieldSize = simulation->getElement(dynamicNeuralFieldCoupling.idsOfCoupledDynamicNeuralFields[0])->getSize();
//        const int x_max = simulation->getElement(dynamicNeuralFieldCoupling.idsOfCoupledDynamicNeuralFields[1])->getMaxSpatialDimension();
//        const double d_x = simulation->getElement(dynamicNeuralFieldCoupling.idsOfCoupledDynamicNeuralFields[1])->getStepSize();
//
//        dynamicNeuralFieldCoupling.gaussianFieldCoupling = std::make_shared<element::GaussFieldCoupling>({ "coupling " + std::to_string(elementCount++), {x_max, d_x} }, architectureParameters.gfcp);
//
//        for (const auto& coupling : dynamicNeuralFieldCoupling.couplings)
//            dynamicNeuralFieldCoupling.gaussianFieldCoupling->addCoupling(coupling);
//
//    }
//
//    void ArchitectureBuilder::addDynamicNeuralFieldCouplingsToSimulation() const
//    {
//        simulation->addElement(dynamicNeuralFieldCoupling.gaussianFieldCoupling);
//    }
//
//    void ArchitectureBuilder::setupInteractionBetweenFields() const
//    {
//        const auto inputField = simulation->getElement(dynamicNeuralFieldCoupling.idsOfCoupledDynamicNeuralFields[0]);
//        const auto outputField = simulation->getElement(dynamicNeuralFieldCoupling.idsOfCoupledDynamicNeuralFields[1]);
//
//        dynamicNeuralFieldCoupling.gaussianFieldCoupling->addInput(inputField);
//        outputField->addInput(dynamicNeuralFieldCoupling.gaussianFieldCoupling);
//    }
//
//    void ArchitectureBuilder::clearDynamicNeuralFieldCouplingsParameters()
//    {
//        dynamicNeuralFieldCoupling = DynamicNeuralFieldCoupling{};
//    }
//}
