cmake_minimum_required(VERSION 3.16)
project(STS_project)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(PROJECT_DIR="${PROJECT_SOURCE_DIR}")

add_subdirectory(json-develop)
add_subdirectory(interactor interactor/interactor_output)