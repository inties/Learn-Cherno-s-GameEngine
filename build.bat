@echo off
echo Building GameEngine with CMake...

:: Create build directory
if not exist "build" mkdir build
cd build

:: Generate Visual Studio project files
cmake .. -G "Visual Studio 17 2022" -A x64

:: Build the project
echo.
echo Building Debug configuration...
cmake --build . --config Debug

echo.
echo Building Release configuration...
cmake --build . --config Release

echo.
echo Build completed! 
echo Debug binaries: build/bin/Debug/
echo Release binaries: build/bin/Release/

pause 