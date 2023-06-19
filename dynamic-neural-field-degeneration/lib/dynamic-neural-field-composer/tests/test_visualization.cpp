#include <catch2/catch_test_macros.hpp>

#include "simulation/visualization.h"
#include "elements/gauss_stimulus.h"

// Helper function to create a sample Element object for testing
std::shared_ptr<GaussStimulus> createSampleElement(const std::string& elementId)
{
    GaussStimulusParameters gsp { 1, 2, 3 };
    return std::make_shared<GaussStimulus>(elementId, 10, gsp);
}

TEST_CASE("Visualization class tests", "[visualization]")
{
    SECTION("Visualization constructor throws exception when given null Simulation pointer")
    {
        REQUIRE_THROWS_AS(Visualization(nullptr), Exception);
    }

    SECTION("Visualization setSimulation method throws exception when given null Simulation pointer")
    {
        Visualization vis(std::make_shared<Simulation>());
        REQUIRE_THROWS_AS(vis.setSimulation(nullptr), Exception);
    }

    SECTION("Visualization addPlottingData method throws exception when data is not found")
    {
        Visualization vis(std::make_shared<Simulation>());
        REQUIRE_THROWS_AS(vis.addPlottingData("invalid_element", "invalid_component"), Exception);
    }

    SECTION("Visualization addPlottingData method doesn't add data when it's already in the vector")
    {
        auto sim = std::make_shared<Simulation>();
        std::shared_ptr<GaussStimulus> gaussStimulus = createSampleElement("element1");
        sim->addElement(gaussStimulus);

        Visualization vis(sim);
        vis.addPlottingData("element1", "output");

        REQUIRE(vis.getNumberOfPlots() == 1);

        // Add the same data again
        vis.addPlottingData("element1", "output");

        REQUIRE(vis.getNumberOfPlots() == 1);
    }

    SECTION("Visualization getPlottingLabel method throws exception when given invalid index")
    {
        auto sim = std::make_shared<Simulation>();
        std::shared_ptr<GaussStimulus> gaussStimulus = createSampleElement("element1");
        sim->addElement(gaussStimulus);

        Visualization vis(sim);
        vis.addPlottingData("element1", "output");

        REQUIRE_THROWS_AS(vis.getPlottingLabel(-1), Exception);
        REQUIRE_THROWS_AS(vis.getPlottingLabel(1), Exception);
    }

    SECTION("Visualization getPlottingData method throws exception when given invalid index")
    {
        auto sim = std::make_shared<Simulation>();
        std::shared_ptr<GaussStimulus> gaussStimulus = createSampleElement("element1");
        sim->addElement(gaussStimulus);

        Visualization vis(sim);
        vis.addPlottingData("element1", "output");

        REQUIRE_THROWS_AS(vis.getPlottingData(-1), Exception);
        REQUIRE_THROWS_AS(vis.getPlottingData(1), Exception);
    }

    SECTION("Visualization getNumberOfPlots method returns the correct number of plots")
    {
        auto sim = std::make_shared<Simulation>();
        std::shared_ptr<GaussStimulus> gaussStimulus1 = createSampleElement("element1");
        std::shared_ptr<GaussStimulus> gaussStimulus2 = createSampleElement("element2");
        sim->addElement(gaussStimulus1);
        sim->addElement(gaussStimulus2);

        Visualization vis(sim);
        vis.addPlottingData("element1", "output");
        vis.addPlottingData("element2", "output");

        REQUIRE(vis.getNumberOfPlots() == 2);
    }
}