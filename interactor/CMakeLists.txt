cmake_minimum_required(VERSION 3.16)
project(interactor)

set(CMAKE_CXX_STANDARD 20)
set(JSON_BuildTests OFF CACHE INTERNAL "")

#find_package(PythonLibs 3.8 REQUIRED)
#find_package(Boost COMPONENTS python REQUIRED)

#set(CMAKE_SHARED_MODULE_PREFIX "")

add_executable(interactor main.cpp env.cpp env.h cards.cpp cards.h player.cpp player.h entity.cpp entity.h mob.cpp mob.h action.cpp action.h random.h random.cpp)
#add_library(interactor MODULE env.cpp env.h cards.cpp cards.h player.cpp player.h entity.cpp entity.h mob.cpp mob.h action.cpp action.h random.h random.cpp wrap.cpp)
#target_link_libraries(interactor nlohmann_json::nlohmann_json ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
#target_include_directories(interactor PRIVATE ${PYTHON_INCLUDE_DIRS})
target_link_libraries(interactor nlohmann_json::nlohmann_json)