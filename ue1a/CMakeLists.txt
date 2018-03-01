cmake_minimum_required(VERSION 3.8)
project(.)

set(CMAKE_C_FLAGS "-D_BSD_SOURCE -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L -D_SVID_SOURCE -std=c99 -pedantic -Wall -g")

set(SOURCE_FILES src/main.c src/mydiff.c src/mydiff.h)
add_executable(mydiff ${SOURCE_FILES})