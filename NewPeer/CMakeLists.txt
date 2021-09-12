cmake_minimum_required(VERSION 3.20)
project(NewPeer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
add_executable(NewPeer main.c peer.c peer.h header.h ds_peer.c ds_peer.h)
