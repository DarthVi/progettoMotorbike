cmake_minimum_required(VERSION 3.15)
project(progettoCar4)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(progettoCar4
        motorbike.cpp
        motorbike.h
        main.cpp
        Makefile
        mesh.cpp
        mesh.h
        point3.h)
