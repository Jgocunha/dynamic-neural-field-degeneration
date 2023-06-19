cmake_minimum_required (VERSION 3.8)

# setup imgui library
set(IMGUI_HEADERS
    ./imgui/imconfig.h
    ./imgui/imgui_internal.h
    ./imgui/imgui.h
    ./imgui/imstb_rectpack.h
    ./imgui/imstb_textedit.h
    ./imgui/imstb_truetype.h    
    ./imgui/backends/imgui_impl_win32.h
    ./imgui/backends/imgui_impl_dx12.h
    ./imgui/misc/cpp/imgui_stdlib.h
)

set(IMGUI_SRC
    ./imgui/imgui_demo.cpp
    ./imgui/imgui_draw.cpp
    ./imgui/imgui_tables.cpp
    ./imgui/imgui_widgets.cpp
    ./imgui/imgui.cpp
    ./imgui/backends/imgui_impl_win32.cpp
    ./imgui/backends/imgui_impl_dx12.cpp
    ./imgui/misc/cpp/imgui_stdlib.cpp
)

add_library(imgui ${IMGUI_HEADERS} ${IMGUI_SRC})

target_link_libraries(imgui PUBLIC "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")
target_compile_definitions(imgui PRIVATE IMGUI_DLL_EXPORT)

include_directories(./imgui/ ./imgui/examples ./imgui/examples/libs/gl3w ./imgui/backends ./imgui/misc/cpp)

#setup implot library
set(IMPLOT_HEADERS ./implot/implot.h ./implot/implot_internal.h)
set(IMPLOT_SRC     ./implot/implot.cpp ./implot/implot_items.cpp ./implot/implot_demo.cpp) 

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../implot/backends")
list(APPEND IMPLOT_HEADERS ./implot/backends/implot_backend.h ./implot/backends/implot_impl_opengl3.h)
list(APPEND IMPLOT_SRC ./implot/backends/implot_backend.h ./implot/backends/implot_impl_opengl3.cpp)
endif()

add_library(implot ${IMPLOT_HEADERS} ${IMPLOT_SRC})
target_link_libraries(implot PUBLIC imgui)
target_compile_definitions(implot PUBLIC IMPLOT_DEBUG IMPLOT_DLL_EXPORT IMPLOT_BACKEND_ENABLE_OPENGL3 IMGUI_IMPL_OPENGL_LOADER_GLAD)
set_property(TARGET implot PROPERTY CXX_STANDARD 11)

include_directories(./implot/)