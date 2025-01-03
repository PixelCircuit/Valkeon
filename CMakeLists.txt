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

# Add Assimp
add_subdirectory(thirdparty/assimp EXCLUDE_FROM_ALL)

# Suppress unknown pragma warnings for Assimp when using g++
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(assimp PRIVATE -Wno-unknown-pragmas)
endif()

# Add Vulkan Memory Allocator (VMA)
add_subdirectory(thirdparty/VulkanMemoryAllocator)

# Define Shaders Source Directory
set(SHADERS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/Shaders")

# Define Shaders Build Directory (temporary directory for compiled shaders)
set(SHADERS_BUILD_DIR "${CMAKE_BINARY_DIR}/shaders_build")

# Define Shaders Destination Directory (where the executable will look for shaders)
set(SHADERS_DEST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders")

# Create the Shaders Build Directory
file(MAKE_DIRECTORY "${SHADERS_BUILD_DIR}")

# Collect all shader source files (*.vert, *.frag, *.comp)
file(GLOB SHADER_SOURCE_FILES
    "${SHADERS_SOURCE_DIR}/*.vert"
    "${SHADERS_SOURCE_DIR}/*.frag"
    "${SHADERS_SOURCE_DIR}/*.comp"
)

# Function to compile a shader
function(compile_shader shader_src shader_out)
    add_custom_command(
        OUTPUT "${shader_out}"
        COMMAND ${GLSLC_EXECUTABLE} -c "${shader_src}" -o "${shader_out}"
        DEPENDS "${shader_src}"
        COMMENT "Compiling ${shader_src} to ${shader_out}"
    )
endfunction()

# Iterate over each shader source file and define its compilation
foreach(SHADER_SRC ${SHADER_SOURCE_FILES})
    get_filename_component(SHADER_NAME "${SHADER_SRC}" NAME)
    set(SHADER_OUT "${SHADERS_BUILD_DIR}/${SHADER_NAME}.spv")
    compile_shader("${SHADER_SRC}" "${SHADER_OUT}")
    list(APPEND COMPILED_SHADERS "${SHADER_OUT}")
endforeach()

# Create a custom target to build all shaders
add_custom_target(CompileShaders ALL DEPENDS ${COMPILED_SHADERS})


# Ensure Shaders Destination Directory exists before copying
add_custom_command(TARGET CompileShaders POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADERS_DEST_DIR}"
    COMMENT "Creating Shaders destination directory: ${SHADERS_DEST_DIR}"
)

# Add custom commands to copy each compiled shader to the destination directory
foreach(COMPILED_SHADER ${COMPILED_SHADERS})
    add_custom_command(TARGET CompileShaders POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${COMPILED_SHADER}" "${SHADERS_DEST_DIR}/"
        COMMENT "Copying ${COMPILED_SHADER} to ${SHADERS_DEST_DIR}/"
    )
endforeach()

add_library(Core STATIC
        Core/VulkanContext.cpp
        Core/Swapchain.cpp
        Core/Pipeline.cpp
        Core/RenderPass.cpp
        Core/Framebuffer.cpp
        Core/CommandPool.cpp
        Core/Synchronization.cpp
        Core/Types.hpp
        Core/Buffer.cpp
        Core/Buffer.hpp
)

# Include directories
target_include_directories(Core PUBLIC
    ${CMAKE_SOURCE_DIR}/Core
    ${Vulkan_INCLUDE_DIRS}              # Vulkan headers
    thirdparty/glm                      # GLM headers
    thirdparty/glfw/include              # GLFW headers
	thirdparty/assimp/include            # Assimp headers
    thirdparty/VulkanMemoryAllocator/include                # VMA headers
)

# Link libraries
target_link_libraries(Core PUBLIC
    Vulkan::Vulkan                       # Vulkan library
    glfw                                # GLFW library
    glm                                 # GLM library (header-only)
	assimp::assimp                      # Assimp library
    VulkanMemoryAllocator               # VMA library
)

add_executable(Valkeon main.cpp
        Applications/HelloTriangleApplication.cpp
        Applications/ModelLoadingApplication.cpp
        Applications/ModelLoadingApplication.hpp
)

target_link_libraries(Valkeon PRIVATE Core)

# Make the executable depend on shader compilation
add_dependencies(Valkeon CompileShaders)

# (Optional) Define any compile definitions or options
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Valkeon PRIVATE ENABLE_VALIDATION_LAYERS)
endif()

# Ensure GLM is available as a target (header-only)
target_link_libraries(Valkeon PRIVATE glm)
