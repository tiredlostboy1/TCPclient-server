cmake_minimum_required(VERSION 3.10)

project(tcp_example)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(server server/server.c)
add_executable(client client/client.c)

target_link_libraries(server pthread)

target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
