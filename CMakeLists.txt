cmake_minimum_required(VERSION 3.31)

set(C_STANDARD 23)
set(C_STANDARD_REQUIRED ON)
set(PROJECT_NAME tetris)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT_VERSION 1.0.0)
set(CMAKE_C_COMPILER clang)

project(${PROJECT_NAME} VERSION ${PROJECT_VERSION})

find_package(SDL3 REQUIRED)

configure_file(src/cmake_variables.h.in ${CMAKE_SOURCE_DIR}/src/_gen/cmake_variables.h @ONLY)

set(SOURCES
  src/main.c)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL3_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} ${SDL3_LIBRARIES})
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
