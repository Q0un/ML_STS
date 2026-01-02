add_library(srcs STATIC
    action.cpp
    cards.cpp
    entity.cpp
    env.cpp
    mob.cpp
    player.cpp
    random.cpp
)

target_include_directories(srcs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(srcs PUBLIC nlohmann_json::nlohmann_json)
