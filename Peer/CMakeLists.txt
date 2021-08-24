cmake_minimum_required(VERSION 3.13)  # CMake version check
project(simple_example)               # Create project "simple_example"
set(CMAKE_C_STANDARD 11)            # Enable c++14 standard

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES main.c peer.c peer.h ds_peer.c ds_peer.h)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(simple_example ${SOURCE_FILES})
