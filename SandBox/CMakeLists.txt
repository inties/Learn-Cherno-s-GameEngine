project(SandBox LANGUAGES CXX)

# Collect all source files
file(GLOB_RECURSE SANDBOX_SOURCES
    "src/*.cpp"
    "src/*.h"
)

# Create console application
add_executable(SandBox ${SANDBOX_SOURCES})

# Set C++ standard
set_target_properties(SandBox PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Include directories
target_include_directories(SandBox PRIVATE
    src
)

# Link directories
target_link_directories(SandBox PRIVATE
    ${CMAKE_SOURCE_DIR}/GameEngine/dependency/GLFW_3_4/lib
    ${CMAKE_SOURCE_DIR}/GameEngine/dependency/libs
)

# Link libraries
target_link_libraries(SandBox
    GameEngine
    glfw3
)

# Windows specific settings
if(WIN32)
    # Compiler definitions
    target_compile_definitions(SandBox PRIVATE
        ENGINE_PLATFORM_WINDOWS
        GLAD_GLAPI_EXPORT
    )
    
    # Build configuration specific definitions
    target_compile_definitions(SandBox PRIVATE
        $<$<CONFIG:Debug>:ENGINE_DEBUG>
        $<$<CONFIG:Release>:ENGINE_RELEASE>
        $<$<CONFIG:RelWithDebInfo>:ENGINE_RELEASE>
        $<$<CONFIG:MinSizeRel>:ENGINE_DIST>
    )
    
    # MSVC specific settings
    if(MSVC)
        target_compile_options(SandBox PRIVATE /utf-8)
        set_target_properties(SandBox PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
        )
    endif()
endif()

# Set output directory to match Premake structure
set_target_properties(SandBox PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/SandBox"
    # Set intermediate directory to match Premake structure
    BINARY_DIR "${CMAKE_SOURCE_DIR}/bin-int/$<CONFIG>-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/SandBox"
)

# Make SandBox depend on GameEngine
add_dependencies(SandBox GameEngine) 