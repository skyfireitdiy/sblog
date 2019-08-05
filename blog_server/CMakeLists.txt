cmake_minimum_required(VERSION 3.14)

project(blog_server)

add_executable(blog_server blog_server.cpp main.cpp config_manager.cpp)

include_directories(../third_part/sflib)

target_link_libraries(blog_server pthread ssl crypto z stdc++fs)

