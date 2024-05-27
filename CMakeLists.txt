cmake_minimum_required(VERSION 3.27)
project(lab4_part2)

set(CMAKE_CXX_STANDARD 17)

add_executable(server server.cpp)

add_executable(client client.cpp)
