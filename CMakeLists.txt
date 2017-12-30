cmake_minimum_required(VERSION 3.9)
project(ue3 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-D_BSD_SOURCE -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L -D_SVID_SOURCE -std=c99 -pedantic -Wall -g")

file(GLOB COMMON_SOURCES src/common/*.c)
file(GLOB SERVER_SOURCES src/server/*.c)
file(GLOB CLIENT_SOURCES src/client/*.c)

add_executable(server ${SERVER_SOURCES} ${COMMON_SOURCES})
add_executable(client ${CLIENT_SOURCES} ${COMMON_SOURCES})