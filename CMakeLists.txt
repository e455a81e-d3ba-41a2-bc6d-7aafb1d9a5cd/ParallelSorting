cmake_minimum_required(VERSION 3.9)
project(ParallelSorting)

find_package(OpenMP REQUIRED)

add_executable(ParallelSorting src/main.cpp src/mergesort.h)
target_include_directories(ParallelSorting PRIVATE src)
target_compile_features(ParallelSorting PRIVATE cxx_std_17)
target_compile_options(ParallelSorting PRIVATE ${OpenMP_CXX_FLAGS})
target_link_libraries(ParallelSorting PRIVATE ${OpenMP_EXE_LINKER_FLAGS})