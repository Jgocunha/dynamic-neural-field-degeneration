cmake_minimum_required(VERSION 3.15)

project(dnf-robustness-adaptability
        DESCRIPTION "Robustness and Adaptability in a Dynamic Neural Field Architecture subject to Degeneration"
        LANGUAGES CXX)
		
# Define library version
set(DNF_DEGENERATION_VERSION_MAJOR 1 CACHE STRING "major version" FORCE)
set(DNF_DEGENERATION_VERSION_MINOR 0 CACHE STRING "minor version" FORCE)
set(DNF_DEGENERATION_VERSION ${DNF_DEGENERATION_VERSION_MAJOR}.${DNF_DEGENERATION_VERSION_MINOR} CACHE STRING "version" FORCE)

# Library build type prefix
set(DNF_DEGENERATION_LIB_PREFIX "release")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DNF_DEGENERATION_LIB_PREFIX "debug")
endif()

# Installation directories
set(DNF_DEGENERATION_INC_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include")
set(DNF_DEGENERATION_RUNTIME_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")
set(DNF_DEGENERATION_LIBRARY_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib/${DNF_DEGENERATION_LIB_PREFIX}")
set(DNF_DEGENERATION_ARCHIVE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib/${DNF_DEGENERATION_LIB_PREFIX}")
set(DNF_DEGENERATION_FRAMEWORK_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")
set(DNF_DEGENERATION_CMAKE_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/${CMAKE_PROJECT_NAME}")
set(DNF_DEGENERATION_ADDITIONAL_FILES_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/${CMAKE_PROJECT_NAME}")


# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# VCPKG integration
if(NOT DEFINED ENV{VCPKG_ROOT})
    message(FATAL_ERROR "ERROR: This project requires VCPKG.\n")
endif()
set(VCPKG_ROOT $ENV{VCPKG_ROOT})
include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

# Project directory definitions
set(PROJECT_DIR "${CMAKE_SOURCE_DIR}")
add_compile_definitions(PROJECT_DIR="${PROJECT_DIR}")

set(OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
add_compile_definitions(OUTPUT_DIRECTORY="${OUTPUT_DIRECTORY}")

# Header and source files
set(header
"include/dnf_architecture.h"
"include/user_interface_window.h"
"include/degenerate_neural_field.h"
"include/degenerate_field_coupling.h"
"include/dnfc_handler_recov.h"
"include/exp_handler_recov.h"
"include/relearning_parameters.h"
"include/degeneration_parameters.h"
"include/experiment_parameters.h"
)

set(src
"src/dnf_architecture.cpp"
"src/user_interface_window.cpp"
"src/degenerate_neural_field.cpp"
"src/degenerate_field_coupling.cpp"
"src/dnfc_handler_recov.cpp"
"src/exp_handler_recov.cpp"
"src/relearning_parameters.cpp"
"src/degeneration_parameters.cpp"
"src/experiment_parameters.cpp"
)

# Library target definition
add_library(${CMAKE_PROJECT_NAME} ${header} ${src})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${DNF_DEGENERATION_INC_INSTALL_DIR}> 
)

# Setup imgui - win32 Directx12
find_package(imgui CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE imgui::imgui)

# Setup implot
find_package(implot CONFIG REQUIRED)

# Setup imgui-node-editor
find_package(unofficial-imgui-node-editor CONFIG REQUIRED)

# Setup nlohmann-json
find_package(nlohmann_json CONFIG REQUIRED)

# Setup imgui-platform-kit
find_package(imgui-platform-kit REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE imgui-platform-kit)

# Setup dynamic-neural-field-composer
find_package(dynamic-neural-field-composer REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE dynamic-neural-field-composer)

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
    DNF_DEGENERATION=1
    DNF_DEGENERATION_VERSION_MAJOR=${DNF_DEGENERATION_VERSION_MAJOR}
    DNF_DEGENERATION_VERSION_MINOR=${DNF_DEGENERATION_VERSION_MINOR}
)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "${CMAKE_PROJECT_NAME}-${DNF_DEGENERATION_VERSION}"
    POSITION_INDEPENDENT_CODE ON
)

# Install the library (binaries)
install(TARGETS ${CMAKE_PROJECT_NAME} EXPORT ${CMAKE_PROJECT_NAME}Targets
    RUNTIME       DESTINATION ${DNF_DEGENERATION_RUNTIME_INSTALL_DIR}
    LIBRARY       DESTINATION ${DNF_DEGENERATION_LIBRARY_INSTALL_DIR}
    ARCHIVE       DESTINATION ${DNF_DEGENERATION_ARCHIVE_INSTALL_DIR}
    FRAMEWORK     DESTINATION ${DNF_DEGENERATION_FRAMEWORK_INSTALL_DIR}
)

# Install the headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${DNF_DEGENERATION_INC_INSTALL_DIR}/dnf-robustness-adaptability
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")

# Package version configuration
include(CMakePackageConfigHelpers)
set(version_file "${CMAKE_CURRENT_BINARY_DIR}/cmake/${CMAKE_PROJECT_NAME}-config-version.cmake")
write_basic_package_version_file(${version_file}
    VERSION ${DNF_DEGENERATION_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(EXPORT ${CMAKE_PROJECT_NAME}Targets DESTINATION ${DNF_DEGENERATION_CMAKE_CONFIG_INSTALL_DIR}
    FILE ${CMAKE_PROJECT_NAME}-config.cmake
)

# Recovering from degeneration
set(RECOV_DEG_EXE recovering-from-degeneration)
add_executable(${RECOV_DEG_EXE} "experiments/recovering-from-degeneration.cpp")
target_include_directories(${RECOV_DEG_EXE} PRIVATE include)
target_link_libraries(${RECOV_DEG_EXE} PRIVATE 
    imgui::imgui 
    imgui-platform-kit 
    dynamic-neural-field-composer 
    ${CMAKE_PROJECT_NAME}
)