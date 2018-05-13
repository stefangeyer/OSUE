cmake_minimum_required(VERSION 3.9)
project(ue2 C)

set(CMAKE_C_STANDARD 99)

add_executable(ue2 src/main.c src/process.c src/process.h src/main.h)