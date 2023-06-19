#include <catch2/catch_test_macros.hpp>

#include "elements/mexican_hat_kernel.h"

TEST_CASE("MexicanHatKernel class tests", "[MexicanHatKernel]")
{
    uint8_t size = 10;
    MexicanHatKernelParameters params;
    params.sigmaExc = 1.0;
    params.amplitudeExc = 2.0;
    params.sigmaInh = 0.5;
    params.amplitudeInh = 1.0;
    params.amplitudeGlobal = 0.1;
    params.fullSum = 0.0;
    params.cutOfFactor = 3;

    SECTION("MexicanHatKernel constructor and getParameters method")
    {
        MexicanHatKernel kernel("test", size, params);

        REQUIRE(kernel.getLabel() == ElementLabel::MEXICAN_HAT_KERNEL);
        REQUIRE(kernel.getUniqueIdentifier() == "test");
        REQUIRE(kernel.getSize() == size);
        REQUIRE(kernel.getParameters() == params);

        REQUIRE(kernel.getComponent("input").size() == size);
        REQUIRE(kernel.getComponent("kernel").size() == size);
        REQUIRE(kernel.getComponent("output").size() == size);
    }

    SECTION("init() method")
    {
        // to do
    }

    SECTION("step() method")
    {
        // to do
	}

    SECTION("setParameters() method")
    {
        MexicanHatKernel kernel("test", size, params);
		params.sigmaExc = 2.0;
		kernel.setParameters(params);
		REQUIRE(kernel.getParameters() == params);
    }
}