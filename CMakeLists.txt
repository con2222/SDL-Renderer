cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(3d-renderer)

include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SDL_TESTS OFF CACHE BOOL "Disable SDL tests" FORCE)
set(SDL_SHARED OFF CACHE BOOL "Build static SDL library instead of shared" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build static SDL" FORCE)
set(PROJECT_NAME 3d-renderer)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.32.10
)

FetchContent_MakeAvailable(SDL2)

set(SOURCES 
    src/main.cpp
    src/display.cpp
    src/mesh.cpp
    src/triangle.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include/)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2-static SDL2::SDL2main)
