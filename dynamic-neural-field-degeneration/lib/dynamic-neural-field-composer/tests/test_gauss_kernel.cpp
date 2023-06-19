#include <catch2/catch_test_macros.hpp>

#include "elements/gauss_kernel.h"

TEST_CASE("GaussKernel class tests", "[GaussKernel]")
{
	uint8_t size = 10;
	GaussKernelParameters params;
	params.sigma = 1.0;
	params.amplitude = 2.0;
	params.cutOfFactor = 3;

	SECTION("GaussKernel constructor and getParameters method")
	{
		GaussKernel kernel("test", size, params);
		REQUIRE(kernel.getLabel() == ElementLabel::GAUSS_KERNEL);
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
		GaussKernel kernel("test", size, params);
		params.sigma = 2.0;
		kernel.setParameters(params);
		REQUIRE(kernel.getParameters() == params);
	}
}