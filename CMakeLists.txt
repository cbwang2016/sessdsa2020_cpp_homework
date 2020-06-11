cmake_minimum_required(VERSION 3.13)
project(final_hw LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -mtune=native -march=native")

find_package(OpenMP REQUIRED)
add_executable(final_hw main.cpp)
target_link_libraries(final_hw PUBLIC OpenMP::OpenMP_CXX)