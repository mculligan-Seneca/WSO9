cmake_minimum_required(VERSION 3.15)
project(in_lab)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(in_lab
        SecureData.cpp
        SecureData.h
        w9.cpp)
