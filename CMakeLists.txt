cmake_minimum_required(VERSION 3.15)
project(GrepCpp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(my_grep src/main.cpp)
