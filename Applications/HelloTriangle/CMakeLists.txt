cmake_minimum_required(VERSION 3.16)

project(Applications)

include(../../CMake/CommonMacros.txt)

SETUP_APP(HelloTriangle "Applications")

target_link_libraries(HelloTriangle PRIVATE SharedUtils)