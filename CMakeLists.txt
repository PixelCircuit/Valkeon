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

add_executable(Valkeon main.cpp
        Core/VulkanContext.cpp
        Core/VulkanContext.hpp
        Core/Swapchain.cpp
        Core/Swapchain.hpp
        Core/Pipeline.cpp
        Core/Pipeline.hpp
        Core/RenderPass.cpp
        Core/RenderPass.hpp
        Core/Framebuffer.cpp
        Core/Framebuffer.hpp
        Core/CommandPool.cpp
        Core/CommandPool.hpp
        Core/Synchronization.cpp
        Core/Synchronization.hpp
        Core/Utils.hpp
)

# Include directories
target_include_directories(Valkeon PRIVATE
    ${Vulkan_INCLUDE_DIRS}              # Vulkan headers
    thirdparty/glm                      # GLM headers
    thirdparty/glfw/include              # GLFW headers
)

# Link libraries
target_link_libraries(Valkeon PRIVATE
    Vulkan::Vulkan                       # Vulkan library
    glfw                                # GLFW library
)

# (Optional) Define any compile definitions or options
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Valkeon PRIVATE ENABLE_VALIDATION_LAYERS)
endif()

# Ensure GLM is available as a target (header-only)
target_link_libraries(Valkeon PRIVATE glm)