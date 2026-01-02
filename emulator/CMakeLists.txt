cmake_minimum_required(VERSION 3.16)
project(emulator)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(PROJECT_DIR="${PROJECT_SOURCE_DIR}")

add_subdirectory(libs)
add_subdirectory(srcs)

add_executable(emulator cmd/main.cpp)
target_link_libraries(emulator srcs)
