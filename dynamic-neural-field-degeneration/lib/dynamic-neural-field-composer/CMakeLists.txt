# CMakeList.txt : CMake project for dynamic-neural-field-composer, include source and define
# project specific logic here.

cmake_minimum_required (VERSION 3.12)

project (dynamic-neural-field-composer 
    VERSION 1.0.0 
    DESCRIPTION "A C++ application to develop Dynamic Neural Field Architectures")

# Add the source files
add_library(dynamic-neural-field-composer 
    "src/simulation/simulation.cpp"
    "src/elements/element.cpp"
    "src/exceptions/exception.cpp"
    "src/elements/neural_field.cpp" 
    "src/mathtools/mathtools.cpp"
    "src/elements/gauss_stimulus.cpp"  
    "src/elements/mexican_hat_kernel.cpp"
    "src/elements/gauss_kernel.cpp"
    "src/elements/normal_noise.cpp"
    "src/simulation/visualization.cpp" 
    "src/application/application.cpp"
    "src/user_interface/user_interface.cpp"
    "src/user_interface/user_interface_window.cpp"
    "examples/ex_shared_task_knowledge.cpp" 
    "src/elements/field_coupling.cpp"
    "src/user_interface/plot_window.cpp"
    "src/user_interface/simulation_window.cpp" 
    "src/user_interface/coupling_window.cpp" 
    "examples/ex_two_layer_field.cpp"
    "src/elements/sum_dimension.cpp" 
    "examples/ex_degeneracy_one_layer_field_model.cpp"
    "src/user_interface/degeneracy_window.cpp" 
    "src/elements/degenerate_neural_field.cpp"
    "src/elements/degenerate_field_coupling.cpp"
    "examples/ex_degeneracy_cuboid_color.cpp" 
    "src/wizards/field_coupling_wizard.cpp"
) 

# Set the desired output directory relative to the project
set(OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/data")

# Pass the OUTPUT_DIRECTORY as a preprocessor definition
add_compile_definitions(OUTPUT_DIRECTORY="${OUTPUT_DIRECTORY}")

# Set C++ standard and compile options
# Set the minimum required C++ version to C++11
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
target_compile_features(dynamic-neural-field-composer PRIVATE cxx_std_20)
#set_property(TARGET dynamic-neural-field-composer PROPERTY CXX_STANDARD 17)
#endif()

# Specify include directories
target_include_directories(dynamic-neural-field-composer PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Main executable
add_executable(launch-dynamic-neural-field-composer 
    "src/dynamic-neural-field-composer.cpp"
)

add_subdirectory(lib)

target_link_libraries(launch-dynamic-neural-field-composer PUBLIC imgui)
target_link_libraries(launch-dynamic-neural-field-composer PUBLIC implot)
target_link_libraries(launch-dynamic-neural-field-composer PRIVATE dynamic-neural-field-composer)

# Set C++ standard and compile options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
target_compile_features(launch-dynamic-neural-field-composer PRIVATE cxx_std_20)
#set_property(TARGET launch-dynamic-neural-field-composer PROPERTY CXX_STANDARD 17)
#if (CMAKE_VERSION VERSION_GREATER 3.12)
#endif() 

# Enable testing
#enable_testing()
# Add Test library
#find_package(Catch2 3 REQUIRED)
#add_subdirectory(lib/Catch2)
#add_subdirectory(tests)
