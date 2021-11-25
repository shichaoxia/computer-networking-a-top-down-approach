cmake_minimum_required(VERSION 3.20)
project(basic C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(basic
        dv.c
        dv.h
        main.c
        node.c
        node.h
        node0.c
        node1.c
        node2.c
        node3.c)
