cmake_minimum_required(VERSION 3.20)

project(jformat LANGUAGES C)

add_executable(${PROJECT_NAME} main.c)

add_definitions(-DSRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(${PROJECT_NAME} PRIVATE
                           ${CMAKE_SOURCE_DIR})
