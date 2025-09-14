# GameEngine Technology Stack

## Build System
- **Primary**: CMake 3.16+ (recommended)
- **Alternative**: Premake5 with Visual Studio project generation
- **Target Platform**: Windows x64 (Visual Studio 2019/2022)
- **C++ Standard**: C++17

## Core Dependencies
- **Graphics API**: OpenGL 3.3+ with GLAD loader
- **Window Management**: GLFW 3.4
- **Mathematics**: GLM (OpenGL Mathematics)
- **ECS Framework**: EnTT (Entity Component System)
- **Model Loading**: Assimp (3D model import)
- **Logging**: spdlog
- **GUI**: Dear ImGui

## Project Structure
- **GameEngine/**: Core engine library (shared DLL)
- **SandBox/**: Example application and testing environment
- **vendor/**: Third-party dependencies
- **文档/**: Chinese documentation and design documents

## Build Commands

### CMake Build (Recommended)
```batch
# Quick build using provided script
build.bat

# Manual build
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Debug
cmake --build . --config Release
```

### Premake Build (Legacy)
```batch
# Generate Visual Studio solution
generateproj.bat
# Then open GameEngine.sln in Visual Studio
```

## Configuration Types
- **Debug**: Development build with debug symbols and assertions
- **Release**: Optimized build for performance testing
- **Dist**: Distribution build (Premake only)

## Output Directories
- **CMake**: `build/bin/<Config>/`
- **Premake**: Root directory
- **Startup Project**: SandBox (main executable)

## Preprocessor Definitions
- `ENGINE_PLATFORM_WINDOWS`: Platform identification
- `ENGINE_BUILDING_DLL`: When building engine as DLL
- `ENGINE_DEBUG/RELEASE/DIST`: Build configuration
- `GLAD_GLAPI_EXPORT`: OpenGL function loading

## Code Conventions
- **Precompiled Headers**: `pch.h` for common includes
- **Namespace**: `Engine` for all engine code
- **Smart Pointers**: Prefer `std::shared_ptr` and `std::unique_ptr`
- **Assertions**: Use `ENGINE_CORE_ASSERT` for engine code
- **Logging**: Use `ENGINE_CORE_INFO/WARN/ERROR` macros