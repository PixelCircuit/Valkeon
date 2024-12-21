cmake_minimum_required(VERSION 3.28)
project(Valkeon)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Specify the output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find Vulkan
find_package(Vulkan REQUIRED)

if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan SDK not found. Please install the Vulkan SDK.")
endif()

# Locate glslc
if (DEFINED ENV{VULKAN_SDK})
    set(VULKAN_SDK_DIR $ENV{VULKAN_SDK})
    set(GLSLC_EXECUTABLE "$ENV{VULKAN_SDK}/bin/glslc")
else()
    find_program(GLSLC_EXECUTABLE glslc)
endif()

if (NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc compiler not found. Please ensure the Vulkan SDK is installed and glslc is in your PATH.")
endif()

message(STATUS "Found glslc: ${GLSLC_EXECUTABLE}")

# Add GLM (header-only library)
add_subdirectory(thirdparty/glm EXCLUDE_FROM_ALL)

# Add GLFW
# Configure GLFW to build as a static library and without examples or tests
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HARFBUZZ OFF CACHE BOOL "" FORCE) # Disable HarfBuzz
add_subdirectory(thirdparty/glfw)


add_library(Core STATIC
        Core/VulkanContext.cpp
        Core/Swapchain.cpp
        Core/Pipeline.cpp
        Core/RenderPass.cpp
        Core/Framebuffer.cpp
        Core/CommandPool.cpp
        Core/Synchronization.cpp
)

# Include directories
target_include_directories(Core PUBLIC
    ${CMAKE_SOURCE_DIR}/Core
    ${Vulkan_INCLUDE_DIRS}              # Vulkan headers
    thirdparty/glm                      # GLM headers
    thirdparty/glfw/include              # GLFW headers
)

# Link libraries
target_link_libraries(Core PUBLIC
    Vulkan::Vulkan                       # Vulkan library
    glfw                                # GLFW library
    glm                                 # GLM library (header-only)
)

add_executable(Valkeon main.cpp
        Applications/HelloTriangleApplication.cpp
)

target_link_libraries(Valkeon PRIVATE Core)

# (Optional) Define any compile definitions or options
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Valkeon PRIVATE ENABLE_VALIDATION_LAYERS)
endif()

# Ensure GLM is available as a target (header-only)
target_link_libraries(Valkeon PRIVATE glm)

# Function to compile all shaders in a directory
function(compile_all_shaders SHADERS_DIR OUTPUT_DIR)
    # Create the output directory
    file(MAKE_DIRECTORY ${OUTPUT_DIR})

    # Find all shader files with extensions .vert, .frag, .geom, .comp, etc.
    file(GLOB_RECURSE SHADER_FILES "${CMAKE_SOURCE_DIR}/${SHADERS_DIR}/*.[vV][eE][rR][tT]?"
            "${CMAKE_SOURCE_DIR}/${SHADERS_DIR}/*.[fF][rR][aA][gG]?"
            "${CMAKE_SOURCE_DIR}/${SHADERS_DIR}/*.[gG][eE][oO][mM]?"
            "${CMAKE_SOURCE_DIR}/${SHADERS_DIR}/*.[cC][oO][mM][pP]?")

    # Initialize an empty list to hold compiled shader paths
    set(COMPILED_SHADERS)

    foreach(SHADER ${SHADER_FILES})
        # Get the relative path of the shader file
        file(RELATIVE_PATH REL_SHADER_PATH "${CMAKE_SOURCE_DIR}/${SHADERS_DIR}" "${SHADER}")

        # Replace backslashes with forward slashes for cross-platform compatibility
        string(REPLACE "\\" "/" REL_SHADER_PATH "${REL_SHADER_PATH}")

        # Extract the filename without extension
        get_filename_component(SHADER_NAME ${SHADER} NAME_WE)

        # Define the output path for the compiled shader
        set(SPIRV_OUTPUT "${OUTPUT_DIR}/${REL_SHADER_PATH}.spv")

        # Create the output subdirectory if necessary
        get_filename_component(SPIRV_DIR "${SPIRV_OUTPUT}" DIRECTORY)
        file(MAKE_DIRECTORY "${SPIRV_DIR}")

        # Add a custom command to compile the shader
        add_custom_command(
                OUTPUT ${SPIRV_OUTPUT}
                COMMAND ${GLSLC_EXECUTABLE} ${SHADER} -o ${SPIRV_OUTPUT}
                DEPENDS ${SHADER}
                COMMENT "Compiling ${REL_SHADER_PATH} to SPIR-V"
                VERBATIM
        )

        # Add the compiled shader to the list
        list(APPEND COMPILED_SHADERS ${SPIRV_OUTPUT})
    endforeach()

    # Add a custom target that depends on all compiled shaders
    add_custom_target(CompileShaders DEPENDS ${COMPILED_SHADERS})

    # Ensure that shaders are compiled before building the executable
    add_dependencies(Valkeon CompileShaders)

    # Copy compiled shaders to the executable's Shaders directory
    foreach(SPIRV ${COMPILED_SHADERS})
        add_custom_command(TARGET Valkeon POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:Valkeon>/${SHADERS_DIR}
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${SPIRV}
                $<TARGET_FILE_DIR:Valkeon>/${SHADERS_DIR}/$(notdir ${SPIRV})
                COMMENT "Copying ${SPIRV} to executable directory"
        )
    endforeach()
endfunction()

# Call the function to compile all shaders in the Shaders directory
compile_all_shaders("Shaders" "${CMAKE_CURRENT_BINARY_DIR}/Shaders")