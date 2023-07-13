#include <catch2/catch_test_macros.hpp>

#include "elements/element.h"
#include "simulation/simulation.h"
#include "elements/neural_field.h"


// Helper function to create a sample Element object for testing
std::shared_ptr<NeuralField> createSampleElement(const std::string& elementId)
{
    NeuralFieldParameters nfp{ 1, -5 };
    ActivationFunctionParameters afp{ ActivationFunctionType::Sigmoid, 1, 0 };
    return std::make_shared<NeuralField>(elementId, 100, nfp, afp);
}


TEST_CASE("Simulation class tests", "[simulation]")
{
    SECTION("Simulation constructor")
    {
        Simulation sim(1, 0, 100);
        REQUIRE(sim.isInitialized() == false);
        REQUIRE(sim.getNumberOfElements() == 0);
        REQUIRE(sim.deltaT == 1);
        REQUIRE(sim.tZero == 0);
        REQUIRE(sim.t == 100);
    }

    SECTION("init method")
    {
		Simulation sim(1, 0, 100);
		sim.init();
		REQUIRE(sim.isInitialized() == true);
        REQUIRE(sim.t == sim.tZero);
	}

    SECTION("step method")
    {
        Simulation sim(1, 0, 100);
        sim.init();
        sim.step();
        REQUIRE(sim.t == 1);
    }

    SECTION("close method")
    {
        Simulation sim(1, 0, 100);
		sim.init();
        REQUIRE(sim.isInitialized() == true);
		sim.close();
		REQUIRE(sim.isInitialized() == false);
    }

    SECTION("run method")
    {
        uint8_t deltaT = 1;
        uint8_t t = 0;
        uint8_t tZero = 100;
        uint8_t runTime = 10;
		Simulation sim(deltaT, t, tZero);
		sim.run(runTime);
		REQUIRE(sim.t == runTime + tZero);
	}

    SECTION("addElement method")
    {
        Simulation sim(1, 0, 100);
        std::shared_ptr<NeuralField> element1 = createSampleElement("Element1");
        std::shared_ptr<NeuralField> element2 = createSampleElement("Element2");

        // Add first element
        sim.addElement(element1);
        REQUIRE(sim.getNumberOfElements() == 1);

        // Add second element
        sim.addElement(element2);
        REQUIRE(sim.getNumberOfElements() == 2);

        // Add element with duplicate ID, should throw exception
        REQUIRE_THROWS_AS(sim.addElement(element1), Exception);
    }

    SECTION("createInteraction method")
    {
        Simulation sim(1, 0, 100);
        std::shared_ptr<NeuralField> stimulusElement = createSampleElement("Element1");
        std::shared_ptr<NeuralField> receivingElement = createSampleElement("Element2");

        // Add elements
        sim.addElement(stimulusElement);
        sim.addElement(receivingElement);

        // Create interaction
        sim.createInteraction(stimulusElement->getUniqueIdentifier(), "output",
            receivingElement->getUniqueIdentifier());

        // Check that the interaction has been created correctly
        REQUIRE(receivingElement->hasInput(stimulusElement->getUniqueIdentifier(), "output"));
    }

    SECTION("removeElement method")
    {
        Simulation sim(1, 0, 100);
        std::shared_ptr<NeuralField> element1 = createSampleElement("Element1");
        std::shared_ptr<NeuralField> element2 = createSampleElement("Element2");

        // Add elements
        sim.addElement(element1);
        sim.addElement(element2);
        
        // Create interaction to test if the input is then removed correctly
        sim.createInteraction(element1->getUniqueIdentifier(), "output",
            element2->getUniqueIdentifier());
        REQUIRE(element2->hasInput(element1->getUniqueIdentifier(), "output") == true);

        // Remove first element
        sim.removeElement(element1->getUniqueIdentifier());
        REQUIRE(sim.getNumberOfElements() == 1);
        REQUIRE(element2->hasInput(element1->getUniqueIdentifier(), "output") == false);


        // Try to remove non-existing element, should throw exception
        REQUIRE_THROWS_AS(sim.removeElement("non_existing_element"), Exception);
    }

    SECTION("resetElement method")
    {
        Simulation sim(1, 0, 100);
        std::shared_ptr<NeuralField> element1 = createSampleElement("Element1");
        std::shared_ptr<NeuralField> element2 = createSampleElement("Element2");

        // Add elements
        sim.addElement(element1);
        sim.addElement(element2);

        // Reset first element with a new element
        std::shared_ptr<NeuralField> newElement = createSampleElement("Element1");
        sim.resetElement(element1->getUniqueIdentifier(), newElement);

        // Check if element is reset
        REQUIRE(sim.getComponent(element1->getUniqueIdentifier(), "output") == sim.getComponent("Element1", "output"));

        // Try to reset non-existing element, should throw exception
        REQUIRE_THROWS_AS(sim.resetElement("non_existing_element", newElement), Exception);
    }

    SECTION("getElement method (with id)")
    {
        Simulation sim(1, 0, 100);
        std::shared_ptr<NeuralField> element1 = createSampleElement("Element1");
        std::shared_ptr<NeuralField> element2 = createSampleElement("Element2");

        // Add elements
        sim.addElement(element1);
        sim.addElement(element2);

        // Test case 1: Get element by valid ID
        std::shared_ptr<Element> foundElement1 = sim.getElement("Element1");
        REQUIRE(foundElement1->getUniqueIdentifier() == "Element1");

        // Test case 2: Get element by invalid ID
        REQUIRE_THROWS_AS(sim.getElement("nonexistentElement"), Exception);
    }

    SECTION("getElement method (with index)")
    {
        Simulation sim(1, 0, 100);
        std::shared_ptr<NeuralField> element1 = createSampleElement("Element1");
        std::shared_ptr<NeuralField> element2 = createSampleElement("Element2");

        // Add elements
        sim.addElement(element1);
        sim.addElement(element2);

        // Test case 1: Get element by valid ID
        std::shared_ptr<Element> foundElement1 = sim.getElement(0);
        REQUIRE(foundElement1->getUniqueIdentifier() == "Element1");

        // Test case 2: Get element by invalid ID
        REQUIRE_THROWS_AS(sim.getElement(2), Exception);
    }

    SECTION("getComponent method")
    {
        Simulation sim(1, 0, 100);
        std::shared_ptr<NeuralField> element1 = createSampleElement("Element1");

        // Add elements
        sim.addElement(element1);

        // Test case 1: Get component by valid ID and component name
        std::vector<double> foundComponent1 = sim.getComponent("Element1", "output");
        REQUIRE(foundComponent1 == std::vector<double>(100, 0.0));

        // Test case 2: Get component by invalid ID
        REQUIRE_THROWS_AS(sim.getComponent("nonexistentElement", "output"), Exception);

        // Test case 3: Get component by invalid component name
        REQUIRE_THROWS_AS(sim.getComponent("Element1", "nonexistentComponent"), Exception);
    }

    SECTION("getComponentPtr method")
    {
        Simulation sim(1, 0, 100);
        std::shared_ptr<NeuralField> element1 = createSampleElement("Element1");

        // Add elements
        sim.addElement(element1);

        // Test case 1: Get component by valid ID and component name
        std::vector<double>* foundComponentPtr1 = sim.getComponentPtr("Element1", "output");
        REQUIRE(*foundComponentPtr1 == std::vector<double>(100, 0.0));

        // Test case 2: Get component by invalid ID
        REQUIRE_THROWS_AS(sim.getComponentPtr("nonexistentElement", "output"), Exception);

        // Test case 3: Get component by invalid component name
        REQUIRE_THROWS_AS(sim.getComponentPtr("Element1", "nonexistentComponent"), Exception);
    }

    SECTION("getNumberOfElements method")
    {
        Simulation sim(1, 0, 100);
        std::shared_ptr<NeuralField> element1 = createSampleElement("Element1");
        std::shared_ptr<NeuralField> element2 = createSampleElement("Element2");

        REQUIRE(sim.getNumberOfElements() == 0);
        sim.addElement(element1);
        REQUIRE(sim.getNumberOfElements() == 1);
        sim.addElement(element2);
        REQUIRE(sim.getNumberOfElements() == 2);

    }

    SECTION("isInitialized method")
    {
        // Create a Simulation object
        Simulation sim;

        // Test the initialization status of the simulation
        REQUIRE(sim.isInitialized() == false);

        // Set the simulation as initialized
        sim.init();

        // Test the updated initialization status of the simulation
        REQUIRE(sim.isInitialized() == true);
    }
}
