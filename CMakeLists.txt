cmake_minimum_required(VERSION 3.16)
project(game_2048 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Find SFML (use system-installed SFML; on macOS: brew install sfml)
# SFML 3.x: component names capitalized, targets are SFML::Graphics etc.
find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)

add_executable(game_2048 src/main.cpp)
target_link_libraries(game_2048 PRIVATE SFML::Graphics SFML::Window SFML::System)

# macOS: app bundle resource path
if(APPLE)
    set_target_properties(game_2048 PROPERTIES
        MACOSX_BUNDLE FALSE
    )
endif()
