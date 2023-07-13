#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "elements/neural_field.h"

TEST_CASE("NeuralField class tests", "[neural_field]")
{
    std::string id = "nf";
    uint8_t size = 10;
    double tau = 1.0;
    double sigmoidSteepness = 2.0;
    double startingRestingLevel = -1.5;
    NeuralFieldParameters nfp{ tau, startingRestingLevel};
    ActivationFunctionParameters afp{ ActivationFunctionType::Sigmoid, sigmoidSteepness, 0 };

	SECTION("NeuralField constructor and getParameters() method")
	{
        NeuralField nf(id, size, nfp, afp);

        REQUIRE(nf.getLabel() == ElementLabel::NEURAL_FIELD);
        REQUIRE(nf.getUniqueIdentifier() == id);
        REQUIRE(nf.getSize() == size);
        REQUIRE(nf.getParameters() == nfp);

        REQUIRE(nf.getComponent("input").size() == size);
        REQUIRE(nf.getComponent("activation").size() == size);
        REQUIRE(nf.getComponent("output").size() == size);
        REQUIRE(nf.getComponent("resting level").size() == size);
	}

    SECTION("init() method")
    {
        NeuralField nf(id, size, nfp, afp);

        nf.init();

        REQUIRE(nf.getComponent("input").size() == size);
        for (const auto& value : nf.getComponent("input"))
            REQUIRE(Catch::Approx(value) == 0.0);


        REQUIRE(nf.getComponent("activation").size() == size);
        for (const auto& value : nf.getComponent("activation"))
            REQUIRE(Catch::Approx(value) == startingRestingLevel);

        REQUIRE(nf.getComponent("output").size() == size);
        //REQUIRE(nf.getComponent("output") == mathtools::sigmoid(nf.getComponent("activation"), nfp.sigmoidSteepness, 0));

        REQUIRE(nf.getComponent("resting level").size() == size);
        for (const auto& value : nf.getComponent("resting level"))
            REQUIRE(Catch::Approx(value) == startingRestingLevel);
    }

    SECTION("step() method") {
        NeuralField nf(id, size, nfp, afp);

        // Run a single step
        nf.step(0, 1);

        //invoked without exceptions
    }

    SECTION("setParameters() method")
    {
        NeuralField nf(id, size, nfp, afp);
        double newtau = 1.5;
        double newsigmoidSteepness = 2.5;
        double newstartingRestingLevel = -1;

        NeuralFieldParameters newnfp{ newtau, newstartingRestingLevel };

        nf.setParameters(newnfp);

        // Check if parameters were updated
        REQUIRE(nf.getParameters() == newnfp);

    }

}