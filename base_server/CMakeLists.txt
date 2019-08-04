cmake_minimum_required(VERSION 3.14)

project(base_server)

add_executable(base_server base_server.cpp)

include_directories(../sflib)

target_link_libraries(base_server pthread ssl crypto z stdc++fs)

