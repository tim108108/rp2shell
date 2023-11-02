cmake_minimum_required(VERSION 3.13)

project(lib)

add_library(lib test.c)
target_link_libraries(lib
    pico_stdlib hardware_pio 
)