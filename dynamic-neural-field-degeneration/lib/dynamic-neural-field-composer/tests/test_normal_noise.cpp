#include <catch2/catch_test_macros.hpp>

#include "elements/normal_noise.h"

TEST_CASE("NormalNoise class tests", "[NormalNoise]")
{
	uint8_t size = 10;
	NormalNoiseParameters params;
	params.amplitude = 1.0;
	SECTION("NormalNoise constructor and getParameters method")
	{
		NormalNoise noise("test", size, params);
		REQUIRE(noise.getLabel() == ElementLabel::NORMAL_NOISE);
		REQUIRE(noise.getUniqueIdentifier() == "test");
		REQUIRE(noise.getSize() == size);
		REQUIRE(noise.getParameters() == params);
		REQUIRE(noise.getComponent("output").size() == size);
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
		NormalNoise noise("test", size, params);
		params.amplitude = 1.5;
		noise.setParameters(params);
		REQUIRE(noise.getParameters() == params);
	}
}