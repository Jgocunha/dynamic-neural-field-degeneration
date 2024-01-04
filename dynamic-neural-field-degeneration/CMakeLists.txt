cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("dynamic-neural-field-degeneration")


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    dynamic-neural-field-degeneration.cpp
)

configure_file(./resources/resources.rc.in ./resources/resources.rc)

# Add main executable
add_executable(launch-dynamic-neural-field-degeneration ${SOURCES} 
    "src/coppeliasim-handler.cpp"
    "src/experiment-handler.cpp"
    "src/dnfcomposer-handler.cpp"
    "src/dnf-architecture.cpp" 
    "src/user-interface-window.cpp"
	./resources/resources.rc
)

# Set the desired output directory relative to the project
set(OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/data")

# Pass the OUTPUT_DIRECTORY as a preprocessor definition
add_compile_definitions(OUTPUT_DIRECTORY="${OUTPUT_DIRECTORY}")

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


configure_file(./resources/resources.rc.in ./resources/resources.rc)

# Add testbench executable

# Add source files
set(SOURCES_
    testbench.cpp

)

add_executable(launch-testbench ${SOURCES_} 
    "src/coppeliasim-handler.cpp"
    "src/experiment-handler.cpp"
    "src/dnfcomposer-handler.cpp"
    "src/dnf-architecture.cpp" 
    "src/user-interface-window.cpp"
	./resources/resources.rc
)

# Set the desired output directory relative to the project
set(OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/data")

# Pass the OUTPUT_DIRECTORY as a preprocessor definition
add_compile_definitions(OUTPUT_DIRECTORY="${OUTPUT_DIRECTORY}")

# Add the include directory of dynamic-neural-field-composer and coppeliasim-cpp-client
target_include_directories(launch-testbench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/dynamic-neural-field-composer/include)
target_include_directories(launch-testbench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/coppeliasim-cpp-client/include)

#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/dynamic-neural-field-composer)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/coppeliasim-cpp-client)

# Link against imgui, implot, dynamic-neural-field-composer, and coppeliasim-cpp-client library
target_link_libraries(launch-testbench PUBLIC imgui)
target_link_libraries(launch-testbench PUBLIC implot)
target_link_libraries(launch-testbench PRIVATE dynamic-neural-field-composer)
target_link_libraries(launch-testbench PRIVATE coppeliasim-cpp-client)
