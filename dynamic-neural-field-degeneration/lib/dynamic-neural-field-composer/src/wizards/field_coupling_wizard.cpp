
#include "./wizards/field_coupling_wizard.h"

FieldCouplingWizard::FieldCouplingWizard(const std::shared_ptr<Simulation> simulation, const std::string& fieldCouplingUniqueId)
	:simulation(simulation)
{
	setFieldCoupling(fieldCouplingUniqueId);
	setNeuralFieldPre();
	setNeuralFieldPost();
    fieldCoupling->resetWeights();

    std::string pathPrefix = std::string(OUTPUT_DIRECTORY) + "/" + fieldCoupling->getUniqueIdentifier() + "_";
    pathToFieldActivationPre = pathPrefix + neuralFieldPre->getUniqueIdentifier() + ".txt";
    pathToFieldActivationPost = pathPrefix + neuralFieldPost->getUniqueIdentifier() + ".txt";
}

void FieldCouplingWizard::setGaussStimulusParameters(const GaussStimulusParameters& gaussStimulusParameters)
{
	this->gaussStimulusParameters = gaussStimulusParameters;
}

void FieldCouplingWizard::setTargetPeakLocationsForNeuralFieldPre(const std::vector<std::vector<double>>& targetPeakLocationsForNeuralFieldPre)
{
	this->targetPeakLocationsForNeuralFieldPre = targetPeakLocationsForNeuralFieldPre;
}

void FieldCouplingWizard::setTargetPeakLocationsForNeuralFieldPost(const std::vector<std::vector<double>>& targetPeakLocationsForNeuralFieldPost)
{
	this->targetPeakLocationsForNeuralFieldPost = targetPeakLocationsForNeuralFieldPost;
}

void FieldCouplingWizard::simulateAssociation()
{
    for (int i = 0; i < targetPeakLocationsForNeuralFieldPre.size(); i++)
    {
        // Create Gaussian stimuli in the input field
        for (int j = 0; j < targetPeakLocationsForNeuralFieldPre[i].size(); j++)
        {
            const std::string stimulusName = "Input Gaussian Stimulus " + std::to_string(i + 1) + std::to_string(j + 1);
            gaussStimulusParameters.position = targetPeakLocationsForNeuralFieldPre[i][j];
            std::shared_ptr<GaussStimulus> stimulus(new GaussStimulus(stimulusName, neuralFieldPre->getSize(), gaussStimulusParameters));
            simulation->addElement(stimulus);
            neuralFieldPre->addInput(stimulus);

            simulation->init();
            fieldCoupling->resetWeights();
            for (int k = 0; k < 100; k++)
                simulation->step();
        }

        // Create Gaussian stimuli in the output field
        for (int j = 0; j < targetPeakLocationsForNeuralFieldPost[i].size(); j++)
        {
            const std::string stimulusName = "Output Gaussian Stimulus " + std::to_string(i + 1) + std::to_string(j + 1);
            gaussStimulusParameters.position = targetPeakLocationsForNeuralFieldPost[i][j];
            std::shared_ptr<GaussStimulus> stimulus(new GaussStimulus(stimulusName, neuralFieldPost->getSize(), gaussStimulusParameters));
            simulation->addElement(stimulus);
            neuralFieldPost->addInput(stimulus);

            simulation->init();
            fieldCoupling->resetWeights();

            for (int k = 0; k < 100; k++)
                simulation->step();
        }

        // Remove gaussian stimuli from the input field
        for (int j = 0; j < targetPeakLocationsForNeuralFieldPre[i].size(); j++)
        {
            std::string stimulusName = "Input Gaussian Stimulus " + std::to_string(i + 1) + std::to_string(j + 1);
            simulation->removeElement(stimulusName);
        }

        // Wait for the input field to settle again
        // simulation->init();
        for (int k = 0; k < 100; k++)
            simulation->step();


        std::vector<double>* input = simulation->getComponentPtr(neuralFieldPre->getUniqueIdentifier(), "activation");
        std::vector<double>* output = simulation->getComponentPtr(neuralFieldPost->getUniqueIdentifier(), "activation");

        const auto inputRestingLevel = simulation->getComponentPtr(neuralFieldPre->getUniqueIdentifier(), "resting level");
        const auto outputRestingLevel = simulation->getComponentPtr(neuralFieldPost->getUniqueIdentifier(), "resting level");

        // normalize data (remove resting level and normalize between -1 and 1))
        *input = normalizeFieldActivation(*input, (*inputRestingLevel)[0]);
        *output = normalizeFieldActivation(*output, (*outputRestingLevel)[0]);

        // save data
        saveFieldActivation(input, pathToFieldActivationPre);
        saveFieldActivation(output, pathToFieldActivationPost);

        // Remove gaussian stimuli from the output field
        for (int j = 0; j < targetPeakLocationsForNeuralFieldPost[i].size(); j++)
        {
            std::string stimulusName = "Output Gaussian Stimulus " + std::to_string(i + 1) + std::to_string(j + 1);
            simulation->removeElement(stimulusName);
        }

        // restart simulation
        simulation->init();
    }
}

std::vector<double> FieldCouplingWizard::normalizeFieldActivation(std::vector<double>& vec, const double& restingLevel)
{
    // this removes the resting level
    // the code works without this  
    //for (double& val : vec)
    //    val += restingLevel;

    for (double& val : vec)
        if (val < 0.01)
            val = 0;

    constexpr int safetyFactor = 0;
    // Find the minimum and maximum values in the vector
    const double maxVal = *std::max_element(vec.begin(), vec.end()) + safetyFactor;
    const double minVal = *std::min_element(vec.begin(), vec.end()) - safetyFactor;

    // Normalize the vector
    std::vector<double> normalizedVec;
    for (double& val : vec)
    {
        if(val != 0.0)
        {
	        //double normalized_val = (val - minVal) / (maxVal - minVal) * 2.0 - 1.0;
            val = (val - minVal) / (maxVal - minVal);
        }

        normalizedVec.push_back(val);
    }

    return normalizedVec;
}

void FieldCouplingWizard::saveFieldActivation(const std::vector<double>* fieldActivation, const std::string& filename)
{
    std::ofstream file(filename, std::ios::app); // Open file in append mode
    if (file.is_open())
    {
        for (const auto& element : (*fieldActivation))
            file << element << " ";  // Write element to file separated by a space
        file << '\n';
        file.close();
    }
    else
    {
        std::cout << "Failed to save data to " << filename << std::endl;
        return;
    }
    //std::cout << "Data saved to " << filename << std::endl;
}

std::vector<double> FieldCouplingWizard::readFieldActivation(const std::string& filename, const int line)
{
    std::ifstream file(filename);
    std::vector<double> data;

    if (file.is_open())
    {
        std::string lineData;
        int currentLine = 0;

        // Read lines from the file until the desired line is reached
        while (std::getline(file, lineData) && currentLine < line)
            currentLine++;

        if (currentLine == line)
        {
            std::istringstream iss(lineData);
            double value;
            while (iss >> value)
                data.push_back(value);
        }
        else
            std::cout << "Line " << static_cast<int>(line) << " not found in " << filename << std::endl;
        file.close();
    }
    else
        std::cout << "Failed to open file " << filename << std::endl;

    return data;
}

void FieldCouplingWizard::trainWeights(const int iterations)
{
    // check how much lines "temp_input.txt", and "temp_output.txt" have
    const int numLinesInput = mathtools::countNumOfLinesInFile(pathToFieldActivationPre);
    const int numLinesOutput = mathtools::countNumOfLinesInFile(pathToFieldActivationPost);

    if (numLinesInput != numLinesOutput)
        std::cerr << "The files " << pathToFieldActivationPre << " and " << pathToFieldActivationPost << " have a different number of lines.\n";

    // read data and update weights
    int lineCount = 0;
    std::vector<double> input = std::vector<double>(neuralFieldPre->getSize());
    std::vector<double> output = std::vector<double>(neuralFieldPost->getSize());
    for (int i = 0; i < iterations; i++)
    {
        input = readFieldActivation(pathToFieldActivationPre, lineCount);
        output = readFieldActivation(pathToFieldActivationPost, lineCount);
        fieldCoupling->updateWeights(input, output);
        lineCount = (lineCount + 1) % (numLinesInput);
    }
}

void FieldCouplingWizard::setFieldCoupling(const std::string& fieldCouplingUniqueId)
{
	std::shared_ptr<Element> fieldCouplingElement = simulation->getElement(fieldCouplingUniqueId);
	fieldCoupling = std::dynamic_pointer_cast<FieldCoupling>(fieldCouplingElement);
}

void FieldCouplingWizard::setNeuralFieldPre()
{
	std::shared_ptr<Element> inputElement = fieldCoupling->getInputs().at(0);
	neuralFieldPre = std::dynamic_pointer_cast<NeuralField>(inputElement);
}

void FieldCouplingWizard::setNeuralFieldPost()
{
	std::shared_ptr<Element> outputElement = simulation->getElementsThatHaveSpecifiedElementAsInput(fieldCoupling->getUniqueIdentifier()).at(0);
	neuralFieldPost = std::dynamic_pointer_cast<NeuralField>(outputElement);
}


void FieldCouplingWizard::saveWeights()
{
    fieldCoupling->saveWeights();
}

void FieldCouplingWizard::clearTargetPeakLocationsFromFiles()
{
    std::ofstream file(pathToFieldActivationPre);
	file.close();
	file.open(pathToFieldActivationPost);
	file.close();
}