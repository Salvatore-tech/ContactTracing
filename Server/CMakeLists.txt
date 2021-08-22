cmake_minimum_required(VERSION 3.20)
project(P2P_server C)

set(CMAKE_C_STANDARD 99)

add_executable(P2P_server main.c server.c server.h)
