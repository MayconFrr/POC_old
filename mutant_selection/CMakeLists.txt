cmake_minimum_required(VERSION 3.16)
project(poc C)

set(CMAKE_C_STANDARD 11)

add_executable(brute_force src/kill_matrix.c src/brute_force.c)
add_executable(cap src/kill_matrix.c src/cap.c)
add_executable(random_cap src/kill_matrix.c src/random_cap.c)
add_executable(smc src/kill_matrix.c src/smc.c)

find_package(OpenMP)
if(OpenMP_C_FOUND)
    target_link_libraries(brute_force PUBLIC OpenMP::OpenMP_C)
    target_link_libraries(cap PUBLIC OpenMP::OpenMP_C)
    target_link_libraries(random_cap PUBLIC OpenMP::OpenMP_C)
    target_link_libraries(smc PUBLIC OpenMP::OpenMP_C)
endif()