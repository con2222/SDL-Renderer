cmake_minimum_required(VERSION 3.20)

project(upng)

add_library(upng STATIC upng.c)

target_include_directories(upng PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)