cmake_minimum_required(VERSION 3.20)
project(Peer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
add_executable(Peer main.c peer.c peer.h common.h ds_peer.c ds_peer.h)
