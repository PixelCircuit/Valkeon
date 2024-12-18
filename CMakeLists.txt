cmake_minimum_required(VERSION 3.28)
project(Valkeon)

set(CMAKE_CXX_STANDARD 20)

add_executable(Valkeon main.cpp
        Core/VulkanContext.cpp
        Core/VulkanContext.hpp
        Core/Swapchain.cpp
        Core/Swapchain.hpp
        Core/Pipeline.cpp
        Core/Pipeline.hpp)
