cmake_minimum_required(VERSION 3.16)
project(poc C)

set(CMAKE_C_STANDARD 11)

add_executable(brute_force kill_matrix/kill_matrix.c brute_force/brute_force.c)
add_executable(cap kill_matrix/kill_matrix.c cap/cap.c)
add_executable(random_cap kill_matrix/kill_matrix.c random_cap/random_cap.c)
add_executable(smc kill_matrix/kill_matrix.c smc/smc.c)

find_package(OpenMP)
if(OpenMP_C_FOUND)
    target_link_libraries(brute_force PUBLIC OpenMP::OpenMP_C)
    target_link_libraries(cap PUBLIC OpenMP::OpenMP_C)
    target_link_libraries(random_cap PUBLIC OpenMP::OpenMP_C)
    target_link_libraries(smc PUBLIC OpenMP::OpenMP_C)
endif()