cmake_minimum_required(VERSION 3.14)

project(client)

find_package(Curses REQUIRED)
find_package(Threads REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(client client.c ../player.h)

target_link_libraries(client ${CURSES_LIBRARIES} Threads::Threads) 
