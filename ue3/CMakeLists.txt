cmake_minimum_required(VERSION 3.9)
project(. C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-D_BSD_SOURCE -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L -D_SVID_SOURCE -std=c99 -pedantic -Wall -g")

file(GLOB COMMON_SOURCES src/common/*.c)
file(GLOB SUPERVISOR_SOURCES src/supervisor/*.c)
file(GLOB GENERATOR_SOURCES src/generator/*.c)

add_executable(supervisor ${SUPERVISOR_SOURCES} ${COMMON_SOURCES})
add_executable(generator ${GENERATOR_SOURCES} ${COMMON_SOURCES})

target_link_libraries(supervisor rt pthread)
target_link_libraries(generator rt pthread)