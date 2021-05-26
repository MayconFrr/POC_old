cmake_minimum_required(VERSION 3.16)
project(poc C)

set(CMAKE_C_STANDARD 11)

add_executable(brute_force src/kill_matrix.c src/brute_force.c)

find_package(OpenMP)
if(OpenMP_C_FOUND)
    target_link_libraries(brute_force PUBLIC OpenMP::OpenMP_C)
endif()