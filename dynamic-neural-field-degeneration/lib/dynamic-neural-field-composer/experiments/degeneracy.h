#pragma once


#include "../include/application/application.h"
#include "../examples/examples.h"
#include <iomanip>


struct DegeneracyExperimentUserParameters
{
    std::shared_ptr<Simulation> simulation;
    ElementDegeneracyType degeneracyType{ ElementDegeneracyType::NONE };
    int numTrials{ 10 };
    std::vector<double> stimulusLocations{ 50 };
    int timeForFieldsToSettle{ 300 };
    bool visualize{ false };
    bool debug{ true };
    bool saveData{ true };
    double weightReductionFactor{ 0.8 };
    std::string idOfFieldToDegenerate{ "field u" };
    std::string idOfFieldToObserve{ "field v" };
};

class DegeneracyExperiment
{
private:
    DegeneracyExperimentUserParameters userParameters;

    std::vector<double> centroidValues;
    bool degenerated;
    int numIterations;
    int maxNumIterations;
    std::string filePath;

    std::shared_ptr<Visualization> visualization;
    std::shared_ptr<Application> application;

    std::shared_ptr<DegenerateNeuralField> inputField;
    std::shared_ptr<DegenerateNeuralField> fieldToDegenerate;
    std::shared_ptr<DegenerateNeuralField> fieldToObserve;
    std::shared_ptr<DegenerateFieldCoupling> fieldCoupling;

public:
    DegeneracyExperiment(const DegeneracyExperimentUserParameters& userParameters)
        :userParameters(userParameters)
    {
        centroidValues = std::vector<double>();
        degenerated = false;
        numIterations = 0;
        filePath = "";

        visualization = std::make_shared<Visualization>(userParameters.simulation);
        visualization->addPlottingData("field u", "activation");
        visualization->addPlottingData("field v", "activation");
        //visualization->addPlottingData("field v", "output");
      

        if(userParameters.idOfFieldToDegenerate == "field u")
            fieldToDegenerate = std::dynamic_pointer_cast<DegenerateNeuralField>(userParameters.simulation->getElement("field u"));
        else
            fieldToDegenerate = std::dynamic_pointer_cast<DegenerateNeuralField>(userParameters.simulation->getElement("field v"));

        if (userParameters.idOfFieldToObserve == "field u")
            fieldToObserve = std::dynamic_pointer_cast<DegenerateNeuralField>(userParameters.simulation->getElement("field u"));
        else
            fieldToObserve = std::dynamic_pointer_cast<DegenerateNeuralField>(userParameters.simulation->getElement("field v"));

        inputField = std::dynamic_pointer_cast<DegenerateNeuralField>(userParameters.simulation->getElement("field u"));
        fieldCoupling = std::dynamic_pointer_cast<DegenerateFieldCoupling>(userParameters.simulation->getElement("u - v"));

        std::vector<std::shared_ptr<Visualization>> visualizations;
        visualizations.push_back(visualization);

        application = std::make_shared<Application>(userParameters.simulation, visualizations, userParameters.visualize);

        fieldCoupling->setWeightReductionFactor(userParameters.weightReductionFactor);
        computeMaxNumOfIterations();
    }
    void run()
    {
        setup();
        loopNumLocations();
        close();
    }
private:
    void computeMaxNumOfIterations()
    {
        switch (userParameters.degeneracyType)
        {
        case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
            maxNumIterations = fieldToDegenerate->getSize() * fieldToObserve->getSize();
            maxNumIterations++; // add one to maxNumIterations to be able to see the field not degenerated
            break;
        case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
            maxNumIterations = std::numeric_limits<int>::max();
            break;
        case ElementDegeneracyType::WEIGHTS_REDUCE:
            maxNumIterations = std::numeric_limits<int>::max();
            break;
        case ElementDegeneracyType::NEURONS_DEACTIVATE:
            maxNumIterations = fieldToDegenerate->getSize();
            break;
        case ElementDegeneracyType::NEURONS_DEACTIVATE_PERCENTAGE:
            maxNumIterations = 10;
            maxNumIterations++; // add one to maxNumIterations to be able to see the field not degenerated
            break;
        case ElementDegeneracyType::WEIGHTS_DEACTIVATE_PERCENTAGE:
            maxNumIterations = 10;
            maxNumIterations++; // add one to maxNumIterations to be able to see the field not degenerated
            break;
        default:
            std::cout << "No experiment selected!" << std::endl;
            break;
        }

    }
    void setup()
    {
        application->init();
    }
    void close()
    {
		application->close();
	}
    void loopNumLocations()
    {
        for(size_t numLocations = 1; numLocations <= userParameters.stimulusLocations.size(); numLocations++)
        {
            setSaveDirectory(numLocations);
            loopNumTrials(numLocations);
        }
    }
    void saveCentroid()
    {
        double centroid = fieldToObserve->calculateCentroid();
        centroidValues.push_back(centroid);
    }
    void loopNumTrials(uint8_t numLocations)
    {
        degenerated = false;

        for (size_t numTrial = 0; numTrial < userParameters.numTrials; numTrial++)
        {
            if (userParameters.debug)
                std::cout << "Trial " << numTrial << " Number of locations " << std::to_string(numLocations) << std::endl;

            addStimulus(numLocations);
            initializeSimulation();
            waitForFieldsToSettle();
            removeStimulus(numLocations);
            waitForFieldsToSettle();
            loopDegeneration();
            cleanUpTrial();
        }
    }
    void loopDegeneration()
    {
        numIterations = 0;

        while (!degenerated)
        {
            saveCentroid();
            checkIfMaximumNumberOfIterationsReached();
            checkIfElementDegenerated();
            applyDegeneration();
            waitForFieldsToSettle();
            numIterations++;
        }
    }
    void checkIfMaximumNumberOfIterationsReached()
    {
        if (userParameters.debug)
            std::cout << "Checking if maximum number of iterations was reached. NumIterations " << std::to_string(numIterations) << std::endl;
        if (numIterations > maxNumIterations)
            degenerated = true;
    }
    void checkIfElementDegenerated()
    {
        if (userParameters.debug)
            std::cout << "Checking if element has degenerated. Centroid " << centroidValues.back() << std::endl;
        if (centroidValues.back() <= 0)
            degenerated = true;
    }
    void applyDegeneration()
    {
        switch (userParameters.degeneracyType)
        {
        case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
            fieldCoupling->setDegeneracyType(ElementDegeneracyType::WEIGHTS_DEACTIVATE);
            fieldCoupling->startDegeneration();
            break;
        case ElementDegeneracyType::WEIGHTS_DEACTIVATE_PERCENTAGE:
            fieldCoupling->setDegeneracyType(ElementDegeneracyType::WEIGHTS_DEACTIVATE_PERCENTAGE);
            fieldCoupling->startDegeneration();
            break;
        case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
            fieldCoupling->setDegeneracyType(ElementDegeneracyType::WEIGHTS_RANDOMIZE);
            fieldCoupling->startDegeneration();
            break;
        case ElementDegeneracyType::WEIGHTS_REDUCE:
            fieldCoupling->setDegeneracyType(ElementDegeneracyType::WEIGHTS_REDUCE);
            fieldCoupling->startDegeneration();
            break;
        case ElementDegeneracyType::NEURONS_DEACTIVATE:
            fieldToDegenerate->setDegeneracyType(ElementDegeneracyType::NEURONS_DEACTIVATE);
            fieldToDegenerate->startDegeneration();
            break;
        case ElementDegeneracyType::NEURONS_DEACTIVATE_PERCENTAGE:
            fieldToDegenerate->setDegeneracyType(ElementDegeneracyType::NEURONS_DEACTIVATE_PERCENTAGE);
            fieldToDegenerate->startDegeneration();
            break;
        default:
            std::cout << "No experiment selected!" << std::endl;
            break;
        }
        if (userParameters.debug)
            std::cout << "Degeneration applied to element\n";
    }
    void addStimulus(uint8_t numLocations)
    {
        for (size_t i = 0; i < numLocations; i++)
        {
            GaussStimulusParameters gsp = { 3, 15, userParameters.stimulusLocations[i] };
            std::shared_ptr<GaussStimulus> stimulus(new GaussStimulus("gauss stimulus " + i, inputField->getSize(), gsp));
            userParameters.simulation->addElement(stimulus);
            if (userParameters.debug)
                std::cout << "Added gauss stimulus " << i << " to sim\n";

            userParameters.simulation->createInteraction("gauss stimulus " + i, "output", "field u");
            if (userParameters.debug)
                std::cout << "Set gauss stimulus " << i << " as input to field\n";
        }
    }
    void initializeSimulation()
    {
        userParameters.simulation->init();
        if (userParameters.debug)
            std::cout << "Initialized simulation\n";
    }
    void waitForFieldsToSettle()
    {
        for (int i = 0; i < userParameters.timeForFieldsToSettle; i++)
            application->step();
        if (userParameters.debug)
            std::cout << "Waited for fields to settle\n";
    }
    void removeStimulus(uint8_t numLocations)
    {
        for (size_t i = 0; i < numLocations; i++)
        {
            userParameters.simulation->removeElement("gauss stimulus " + i);
            if (userParameters.debug)
                std::cout << "Removed gauss stimulus " << i << " from simulation\n";
        }
    }
    void cleanUpTrial()
    {
        degenerated = false;
        if(userParameters.saveData)
            saveCentroids();
        centroidValues.clear();
        userParameters.simulation->close();
        if (userParameters.debug)
            std::cout << "Saved centroids, cleared centroids, closed simulation\n\n";
    }
    void saveCentroids()
    {
        std::ofstream outputFile(filePath, std::ios::app); // Open the file in append mode

        if (!outputFile)
            std::cerr << "Failed to open the file for writing." << std::endl;

        for (const auto& centroid : centroidValues)
            outputFile << centroid << " ";
        outputFile << std::endl;

        outputFile.close();

        std::cout << "New centroids appended to " << filePath << std::endl;
    }
    void setSaveDirectory(uint8_t numLocation)
    {
        filePath = std::string(OUTPUT_DIRECTORY) + "/degeneracy_experiment";

        std::stringstream weightReductionFactorStr;
        weightReductionFactorStr << std::fixed << std::setprecision(1) << userParameters.weightReductionFactor;
        
        switch (userParameters.degeneracyType)
        {
        case ElementDegeneracyType::WEIGHTS_DEACTIVATE:
            filePath = filePath + "/WEIGHTS_DEACTIVATE/p" + std::to_string(numLocation) + "/centroids.txt";
            break;
        case ElementDegeneracyType::WEIGHTS_RANDOMIZE:
            filePath = filePath + "/WEIGHTS_RANDOMIZE/p" + std::to_string(numLocation) + "/centroids.txt";
            break;
        case ElementDegeneracyType::WEIGHTS_REDUCE:
            filePath = filePath + "/WEIGHTS_REDUCE_" + std::to_string(userParameters.weightReductionFactor) + "/p" + std::to_string(numLocation) + "/centroids.txt";
            break;
        case ElementDegeneracyType::WEIGHTS_DEACTIVATE_PERCENTAGE:
            filePath = filePath + "/WEIGHTS_DEACTIVATE_PERCENTAGE/p" + std::to_string(numLocation) + "/centroids.txt";
            break;
        case ElementDegeneracyType::NEURONS_DEACTIVATE:
            filePath = filePath + "/NEURONS_DEACTIVATE_" + userParameters.idOfFieldToDegenerate + "_" + userParameters.idOfFieldToObserve + "/p" + std::to_string(numLocation) + "/centroids.txt";
            break;
        case ElementDegeneracyType::NEURONS_DEACTIVATE_PERCENTAGE:
            filePath = filePath + "/NEURONS_DEACTIVATE_PERCENTAGE_" + userParameters.idOfFieldToDegenerate + "_" + userParameters.idOfFieldToObserve + "/p" + std::to_string(numLocation) + "/centroids.txt";
            break;
        default:
            std::cout << "No experiment selected!" << std::endl;
            break;
        }
    }
};