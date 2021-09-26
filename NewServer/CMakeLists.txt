cmake_minimum_required(VERSION 3.20)
project(NewServer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
add_executable(NewServer main.c server.c server.h)
