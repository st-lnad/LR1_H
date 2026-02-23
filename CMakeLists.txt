cmake_minimum_required(VERSION 4.1)
project(LR1_Hammurabi)

set(CMAKE_CXX_STANDARD 20)

add_executable(LR1_Hammurabi main.cpp
        Random.cpp
        Random.h
        Parser.h
)
