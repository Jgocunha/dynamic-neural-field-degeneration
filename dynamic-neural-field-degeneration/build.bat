@echo off

set VCPKG_ROOT=%VCPKG_ROOT%
set PROJECT_ROOT=%CD%

:: Check if the environment variable is set
IF NOT DEFINED VCPKG_ROOT (
    echo ERROR: The environment variable VCPKG_ROOT is not set.
    echo Download and install VCPKG from https://github.com/microsoft/vcpkg#quick-start-windows.
    echo Create an environment variable VCPKG_ROOT that points to the installation directory.
	pause
    exit /b 1
)

:: Install catch2 using Vcpkg
"%VCPKG_ROOT%\vcpkg.exe" install catch2:x64-windows

:: Install imgui using Vcpkg
"%VCPKG_ROOT%\vcpkg.exe" install imgui[docking-experimental,core,dx12-binding,win32-binding]:x64-windows

:: Install implot using Vcpkg
"%VCPKG_ROOT%\vcpkg.exe" install implot:x64-windows

:: Install imgui-node-editor using Vcpkg
"%VCPKG_ROOT%\vcpkg.exe" install imgui-node-editor:x64-windows

:: Install nlohmann-json using Vcpkg
"%VCPKG_ROOT%\vcpkg.exe" install nlohmann-json:x64-windows

:: Using MSBuild may require elevation
"%VCPKG_ROOT%\vcpkg.exe" integrate install

:: Create build folders
mkdir %PROJECT_ROOT%\build\x64-release
mkdir %PROJECT_ROOT%\build\x64-debug

:: Run CMake
cmake -G "Visual Studio 17 2022" -A x64 -S "%PROJECT_ROOT%" -B "%PROJECT_ROOT%\build\x64-release" -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" -DCMAKE_BUILD_TYPE=Release

:: Build your project using the generated build files 
cmake --build "%PROJECT_ROOT%\build\x64-release" --config Release

:: Run CMake
cmake -G "Visual Studio 17 2022" -A x64 -S "%PROJECT_ROOT%" -B "%PROJECT_ROOT%\build\x64-debug" -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" -DCMAKE_BUILD_TYPE=Debug

:: Build your project using the generated build files 
cmake --build "%PROJECT_ROOT%\build\x64-debug" --config Debug


:: Optionally, you can run your executable here if applicable

pause
exit /b 1
