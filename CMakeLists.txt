cmake_minimum_required(VERSION 3.14)

project(server)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(server main.c signature.h)

target_link_libraries(server ${CURSES_LIBRARIES})
