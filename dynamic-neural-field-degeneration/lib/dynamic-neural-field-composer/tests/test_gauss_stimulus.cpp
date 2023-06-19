#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "elements/gauss_stimulus.h"

TEST_CASE("GaussStimulus class tests", "[GaussStimulus]")
{
    // Create a GaussStimulus object for testing
    std::string id = "testGaussStimulus";
    uint8_t size = 3;
    double sigma = 1.0;
    double amplitude = 2.0;
    double position = 0.5;
    GaussStimulusParameters gsp{sigma, amplitude, position};

    SECTION("GaussStimulus constructor getParameters() method")
    {
        GaussStimulus gaussStimulus(id, size, gsp);

        REQUIRE(gaussStimulus.getLabel() == ElementLabel::GAUSS_STIMULUS);
        REQUIRE(gaussStimulus.getUniqueIdentifier() == id);
        REQUIRE(gaussStimulus.getSize() == size);
        REQUIRE(gaussStimulus.getComponent("output").size() == size);

        REQUIRE(gaussStimulus.getParameters() == gsp);

        // invalid position
        gsp.position = position + size;
        REQUIRE_THROWS_AS(GaussStimulus::GaussStimulus(id, size, gsp), Exception);
    }

    SECTION("init() method")
    {
        GaussStimulus gaussStimulus(id, size, gsp);

        // Call init() method
        gaussStimulus.init();

        // Check if components["output"] has correct values
        REQUIRE(gaussStimulus.getComponent("output")[0] == Catch::Approx(amplitude * mathtools::circularGauss(size, sigma, position)[0]));
        REQUIRE(gaussStimulus.getComponent("output")[1] == Catch::Approx(amplitude * mathtools::circularGauss(size, sigma, position)[1]));
        REQUIRE(gaussStimulus.getComponent("output")[2] == Catch::Approx(amplitude * mathtools::circularGauss(size, sigma, position)[2]));
    }

    SECTION("setParameters() method")
    {
        GaussStimulus gaussStimulus(id, size, gsp);
        double newSigma = 1.1;
        double newAmplitude = 3.0;
        double newPosition = 1.5;
        GaussStimulusParameters newgsp{ newSigma, newAmplitude, newPosition};

        gaussStimulus.setParameters(newgsp);

        // Check if parameters were updated and init() was called
        REQUIRE(gaussStimulus.getParameters() == newgsp);
        REQUIRE(gaussStimulus.getComponent("output")[0] == Catch::Approx(newAmplitude * mathtools::circularGauss(size, newSigma, newPosition)[0]));
        REQUIRE(gaussStimulus.getComponent("output")[1] == Catch::Approx(newAmplitude * mathtools::circularGauss(size, newSigma, newPosition)[1]));
        REQUIRE(gaussStimulus.getComponent("output")[2] == Catch::Approx(newAmplitude * mathtools::circularGauss(size, newSigma, newPosition)[2]));
    }

    SECTION("+operator() overload")
    {
        // Create two GaussStimulus objects with the same size and parameters
        GaussStimulus gaussStimulus1("id1", 10, gsp);
        GaussStimulus gaussStimulus2("id2", 10, gsp);

        // Initialize the GaussStimulus objects
        gaussStimulus1.init();
        gaussStimulus2.init();

        // Get the components["output"] vectors
        std::vector<double>& output1 = gaussStimulus1.getComponent("output");
        std::vector<double>& output2 = gaussStimulus2.getComponent("output");


        // Sum the two GaussStimulus objects using the operator+
        GaussStimulus gaussStimulusSum = gaussStimulus1 + gaussStimulus2;
        //gaussStimulusSum.init();
        // Check that the components["output"] vector of the resulting GaussStimulus object is the element-wise sum
        // of the components["output"] vectors of the input GaussStimulus objects
        std::vector<double> outputSum = gaussStimulusSum.getComponent("output");
        REQUIRE(outputSum.size() == output1.size());
        for (size_t i = 0; i < outputSum.size(); i++)
        {
            REQUIRE(outputSum[i] == output1[i] + output2[i]);
        }

        // Create two GaussStimulus objects with the different sizes
        GaussStimulus gaussStimulus3("id1", 11, gsp);
        GaussStimulus gaussStimulus4("id2", 10, gsp);
        REQUIRE_THROWS_AS(gaussStimulus3 + gaussStimulus4, Exception);
    }
}