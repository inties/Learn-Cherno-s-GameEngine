project(Glad LANGUAGES C)

# Create static library
add_library(Glad STATIC
    include/glad/glad.h
    include/KHR/khrplatform.h
    src/glad.c
)

# Set target properties
target_include_directories(Glad PUBLIC include)

# Windows specific settings
if(WIN32)
    target_compile_definitions(Glad PUBLIC
        GLAD_GLAPI_EXPORT
        GLAD_GLAPI_EXPORT_BUILD
    )
    
    # Set runtime library for MSVC
    if(MSVC)
        set_target_properties(Glad PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
        )
    endif()
endif()

# Configuration specific settings
target_compile_definitions(Glad PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<CONFIG:RelWithDebInfo>:NDEBUG>
    $<$<CONFIG:MinSizeRel>:NDEBUG>
)

# Set output directory to match premake structure
set_target_properties(Glad PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib/Debug"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib/Release"
    ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/lib/RelWithDebInfo"
    ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/lib/MinSizeRel"
) 