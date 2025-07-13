cmake_minimum_required(VERSION 3.16)

project(GameEngine LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Platform detection
if(WIN32)
    set(ENGINE_PLATFORM_WINDOWS TRUE)
endif()

# Set runtime library
if(WIN32 AND MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# For multi-config generators
foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG_TYPE} CONFIG_TYPE_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} ${CMAKE_BINARY_DIR}/bin/${CONFIG_TYPE})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} ${CMAKE_BINARY_DIR}/bin/${CONFIG_TYPE})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} ${CMAKE_BINARY_DIR}/lib/${CONFIG_TYPE})
endforeach()

# Include directories
set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/GameEngine/dependency/GLFW_3_4/include")
set(GLAD_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/GameEngine/dependency/glad/include")
set(IMGUI_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/GameEngine/dependency/imgui")
set(GLM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/GameEngine/dependency/glm_99")
set(SPDLOG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/GameEngine/dependency/spdlog/include")

# Library directories
set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/GameEngine/dependency/GLFW_3_4/lib")

# Add subdirectories for dependencies
add_subdirectory(GameEngine/dependency/glad)
add_subdirectory(GameEngine/dependency/imgui)

# Add main projects
add_subdirectory(GameEngine)
add_subdirectory(SandBox)

# Set startup project for Visual Studio
if(WIN32 AND MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SandBox)
endif() 