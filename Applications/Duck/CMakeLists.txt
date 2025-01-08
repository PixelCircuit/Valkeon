cmake_minimum_required(VERSION 3.16)

project(Applications)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Duck "Applications")

target_link_libraries(Duck PRIVATE SharedUtils)