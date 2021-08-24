cmake_minimum_required(VERSION 3.20)
project(ContactTracing)
set(CMAKE_CXX_STANDARD 11)

add_executable(P2P_server main.c server.c server.h)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
