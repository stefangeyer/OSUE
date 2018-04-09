cmake_minimum_required(VERSION 3.9)
project(ue1b C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-D_BSD_SOURCE -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L -D_SVID_SOURCE -std=c99 -pedantic -Wall -g")

set(SOURCES_SERVER src/server.c src/common.h)
set(SOURCES_CLIENT src/client.c src/common.h)
add_executable(server ${SOURCES_SERVER})
add_executable(client ${SOURCES_CLIENT})