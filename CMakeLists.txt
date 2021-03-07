cmake_minimum_required(VERSION 3.17)
project(STS_project)

set(CMAKE_CXX_STANDARD 20)
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json-develop)

add_executable(STS_project main.cpp interactor/env.cpp interactor/env.h interactor/cards.cpp interactor/cards.h interactor/player.cpp interactor/player.h interactor/entity.cpp interactor/entity.h interactor/mob.cpp interactor/mob.h interactor/action.cpp interactor/action.h interactor/random.h)
target_link_libraries(STS_project nlohmann_json::nlohmann_json)

include_directories(interactor)
