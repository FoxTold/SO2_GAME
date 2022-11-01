cmake_minimum_required(VERSION 3.13)

project(server)

find_package(Curses REQUIRED)
find_package(Threads REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(server main.cpp signature.h beast.h player.h common.h)

target_link_libraries(server ${CURSES_LIBRARIES} Threads::Threads) 

add_subdirectory(client)
