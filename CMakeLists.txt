cmake_minimum_required(VERSION 3.27.4)

project(CHIP_8_EMULATOR VERSION 1.0 LANGUAGES C)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/read_file)
include_directories(${CMAKE_SOURCE_DIR}/games)

file(GLOB all_files
    "${PROJECT_SOURCE_DIR}/*"
    "${PROJECT_SOURCE_DIR}/games/*"
    "${PROJECT_SOURCE_DIR}/read_file/*"
)

add_executable(${PROJECT_NAME} main.c ${all_files})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES}) 