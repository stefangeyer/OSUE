cmake_minimum_required(VERSION 3.8)
project(ue1b)

set(CMAKE_C_FLAGS "-D_BSD_SOURCE -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L -D_SVID_SOURCE -std=c99 -pedantic -Wall -g")

set(SOURCE_FILES src/main.c src/server.c src/common.h)
add_executable(battleship ${SOURCE_FILES})