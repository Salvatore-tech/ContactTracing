cmake_minimum_required(VERSION 3.20)
project(P2P C)               # Create project "simple_example"
set(CMAKE_C_STANDARD 99)


# Add main.cpp file of project root directory as source file
set(SOURCE_FILES main.c peer.c peer.h header.h ds_peer.c ds_peer.h)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

include_directories(.)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(P2P ${SOURCE_FILES})
