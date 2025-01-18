cmake_minimum_required(VERSION 3.26)

project(chip8)

include(FetchContent)

set(CHIP8_SOURCES
    "src/main.cpp"
    "src/chip8.cpp"
)

add_executable(chip8 ${CHIP8_SOURCES})

message(">> Fetching SDL")
FetchContent_Declare(sdl
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
    GIT_TAG "origin/SDL2"
    SYSTEM
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(sdl)
target_link_libraries(chip8 PRIVATE SDL2::SDL2)