cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("dynamic-neural-field-degeneration")


# Add source files
set(SOURCES
    dynamic-neural-field-degeneration.cpp
)

# Add executable
add_executable(launch-dynamic-neural-field-degeneration ${SOURCES} "src/threadHandler.cpp" "src/coppeliasimHandler.cpp" "src/dnfcomposerHandler.cpp" "src/dnfarchitecture.cpp")

# Add the include directory of dynamic-neural-field-composer and coppeliasim-cpp-client
target_include_directories(launch-dynamic-neural-field-degeneration PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/dynamic-neural-field-composer/include)
target_include_directories(launch-dynamic-neural-field-degeneration PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/coppeliasim-cpp-client/include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/dynamic-neural-field-composer)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/coppeliasim-cpp-client)

# Link against imgui, implot, dynamic-neural-field-composer, and coppeliasim-cpp-client library
target_link_libraries(launch-dynamic-neural-field-degeneration PUBLIC imgui)
target_link_libraries(launch-dynamic-neural-field-degeneration PUBLIC implot)
target_link_libraries(launch-dynamic-neural-field-degeneration PRIVATE dynamic-neural-field-composer)
target_link_libraries(launch-dynamic-neural-field-degeneration PRIVATE coppeliasim-cpp-client)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET launch-dynamic-neural-field-degeneration PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests
