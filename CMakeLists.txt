cmake_minimum_required(VERSION 3.31)
project(tetris VERSION 1.0)

set(C_STANDARD 23)
set(C_STANDARD_REQUIRED ON)
set(PROJECT_NAME tetris)

find_package(SDL3 REQUIRED)
include_directories(${SDL3_INCLUDE_DIRS} include)

set(SOURCES
  src/main.c)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${SDL3_LIBRARIES})
