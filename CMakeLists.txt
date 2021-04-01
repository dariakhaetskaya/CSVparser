cmake_minimum_required(VERSION 3.16)
project(CSVparser)

set(CMAKE_CXX_STANDARD 17)

add_executable(CSVparser main.cpp tupleExt.h csvparser.h)
