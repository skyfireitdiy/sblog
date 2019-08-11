cmake_minimum_required(VERSION 3.14)
project(blog_server)

set(CMAKE_CXX_STANDARD 20)

file(GLOB sources src/*)

add_executable(blog_server ${sources})

include_directories(
        third_part/sflib
        third_part/sqlite_orm/include/sqlite_orm
        third_part/digestpp
        include
)

target_link_libraries(blog_server pthread ssl crypto z stdc++fs sqlite3)