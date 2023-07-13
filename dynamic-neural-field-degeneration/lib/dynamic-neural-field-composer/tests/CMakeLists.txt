# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.8)

find_package(Catch2 3 REQUIRED)


# Add the test source files
add_library(test_sources OBJECT
	"test_simulation.cpp"
	"test_element.cpp"
	"test_neural_field.cpp"
	"test_gauss_stimulus.cpp"
	"test_mexican_hat_kernel.cpp"
	"test_gauss_kernel.cpp" 
	"test_normal_noise.cpp"
	"test_visualization.cpp"
	"test_application.cpp"
	"test_user_interface.cpp"
	"test_simulation.cpp"
)
target_link_libraries(test_sources PRIVATE
	Catch2::Catch2WithMain
	dynamic-neural-field-composer
)

# Add the test executable
add_executable(tests 
	"test.cpp"
)
# These tests can use the Catch2-provided main
target_link_libraries(tests PRIVATE
	Catch2::Catch2WithMain
	test_sources
	dynamic-neural-field-composer
)
include(CTest)
include(Catch)
catch_discover_tests(tests)

# Set C++ standard and compile options
#if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET tests PROPERTY CXX_STANDARD 20)
#endif()

#add_test(
#	NAME tests
#	COMMAND tests
#)