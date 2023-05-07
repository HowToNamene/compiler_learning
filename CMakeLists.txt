cmake_minimum_required(VERSION 3.24)
project(custom_compiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(custom_compiler main.cpp lexer/lexer.h)
