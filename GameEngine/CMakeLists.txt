project(GameEngine LANGUAGES CXX)

# Collect all source files
file(GLOB_RECURSE GAMEENGINE_SOURCES
    "src/*.cpp"
    "src/*.h"
)

# Create shared library
add_library(GameEngine SHARED ${GAMEENGINE_SOURCES})

# Set C++ standard
set_target_properties(GameEngine PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Set up precompiled headers
target_precompile_headers(GameEngine PRIVATE src/pch.h)

# Include directories
target_include_directories(GameEngine PUBLIC
    src
    ${CMAKE_SOURCE_DIR}/GameEngine/dependency/spdlog/include
    ${CMAKE_SOURCE_DIR}/GameEngine/dependency/include
    ${CMAKE_SOURCE_DIR}/GameEngine/dependency/GLFW_3_4/include
    ${CMAKE_SOURCE_DIR}/GameEngine/dependency/glad/include
    ${CMAKE_SOURCE_DIR}/GameEngine/dependency/imgui
    ${CMAKE_SOURCE_DIR}/GameEngine/dependency/glm_99
    ${CMAKE_SOURCE_DIR}/GameEngine/dependency/entt/src/entt
    ${CMAKE_SOURCE_DIR}/GameEngine/dependency/nlohmann
)

# Link directories
target_link_directories(GameEngine PRIVATE
    dependency/GLFW_3_4/lib
    dependency/libs
)

# Link libraries
target_link_libraries(GameEngine
    ImGui
    Glad
)

# Windows specific settings
if(WIN32)
    # Link Windows libraries
    target_link_libraries(GameEngine
        glfw3
        opengl32
        user32
        gdi32
        shell32
        kernel32
        assimp-vc143-mtd
    )
    
    # Compiler definitions - PUBLIC for consumers, PRIVATE for internal
    target_compile_definitions(GameEngine PUBLIC
        ENGINE_PLATFORM_WINDOWS
        GLAD_GLAPI_EXPORT
        GLAD_GLAPI_EXPORT_BUILD
    )
    
    # This should only be defined when building the DLL, not when using it
    target_compile_definitions(GameEngine PRIVATE
        ENGINE_BUILDING_DLL
    )
    
    # Build configuration specific definitions
    target_compile_definitions(GameEngine PRIVATE
        $<$<CONFIG:Debug>:ENGINE_DEBUG>
        $<$<CONFIG:Release>:ENGINE_RELEASE>
        $<$<CONFIG:RelWithDebInfo>:ENGINE_RELEASE>
        $<$<CONFIG:MinSizeRel>:ENGINE_DIST>
    )
    
    # MSVC specific settings
    if(MSVC)
        target_compile_options(GameEngine PRIVATE /utf-8)
        set_target_properties(GameEngine PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
        )
    endif()
    
    # Post-build step to copy DLL to SandBox directory
    add_custom_command(TARGET GameEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory 
            "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/SandBox"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:GameEngine>"
            "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/SandBox/"
        COMMENT "Copying GameEngine DLL to SandBox directory"
    )
endif()

# Set output directory to match Premake structure
set_target_properties(GameEngine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/GameEngine"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/GameEngine"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/GameEngine"
    # Set intermediate directory to match Premake structure
    BINARY_DIR "${CMAKE_SOURCE_DIR}/bin-int/$<CONFIG>-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/GameEngine"
) 