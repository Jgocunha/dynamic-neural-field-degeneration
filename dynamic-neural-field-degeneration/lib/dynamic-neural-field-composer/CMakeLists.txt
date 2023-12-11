# CMakeList.txt : CMake project for dynamic-neural-field-composer, include source and define
# project specific logic here.

cmake_minimum_required (VERSION 3.15)

project (dynamic-neural-field-composer 
    VERSION 1.0.0 
    DESCRIPTION "A C++ application to develop Dynamic Neural Field Architectures")

configure_file(./resources/resources.rc.in ./resources/resources.rc)

# Add the source files
add_library(dynamic-neural-field-composer 	
	"src/simulation/simulation.cpp"
    "src/simulation/visualization.cpp" 
    
    "src/application/application.cpp"
    
    "src/elements/element_factory.cpp"
    "src/elements/element.cpp"
    "src/elements/field_coupling.cpp"
    "src/elements/neural_field.cpp" 
    "src/elements/gauss_stimulus.cpp"  
    "src/elements/mexican_hat_kernel.cpp"
    "src/elements/gauss_kernel.cpp"
    "src/elements/normal_noise.cpp"
    "src/elements/gauss_field_coupling.cpp" 
    "src/elements/kernel.cpp"
    
    "src/mathtools/mathtools.cpp"

    "src/exceptions/exception.cpp"
    
    "src/user_interface/user_interface.cpp"
    "src/user_interface/user_interface_window.cpp"
    "src/user_interface/plot_window.cpp"
    "src/user_interface/simulation_window.cpp"
    "src/user_interface/logger_window.cpp"

    "src/architecture/architecture_builder.cpp"
    "src/architecture/architecture_file_handler.cpp"

    "src/wizards/learning_wizard.cpp"
	
	"src/elements/degenerate_neural_field.cpp"
	"src/elements/degenerate_field_coupling.cpp"
	
	./resources/resources.rc
) 


# Set the desired output directory relative to the project
set(OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
set(PROJECT_DIR "${CMAKE_SOURCE_DIR}")

# Pass the OUTPUT_DIRECTORY as a preprocessor definition
add_compile_definitions(OUTPUT_DIRECTORY="${OUTPUT_DIRECTORY}")
add_compile_definitions(PROJECT_DIR="${PROJECT_DIR}")

# Set C++ standard and compile options
# Set the minimum required C++ version to C++11
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
target_compile_features(dynamic-neural-field-composer PRIVATE cxx_std_20)

# Specify include directories
target_include_directories(dynamic-neural-field-composer PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(lib)
