cmake_minimum_required(VERSION 3.20)
project(NewServer C)

set(CMAKE_C_STANDARD 11)

add_executable(NewServer main.c server.c server.h)
