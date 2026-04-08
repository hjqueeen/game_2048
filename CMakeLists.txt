cmake_minimum_required(VERSION 3.16)
project(game_2048 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find SFML (use system-installed SFML; on macOS: brew install sfml)
# SFML 3.x: component names capitalized, targets are SFML::Graphics etc.
find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)

add_executable(game_2048
    src/main.cpp
    src/game2048.cpp
    src/game_view.cpp
)
target_link_libraries(game_2048 PRIVATE SFML::Graphics SFML::Window SFML::System)

target_include_directories(game_2048 PRIVATE src)

# Copy assets folder next to executable after build.
add_custom_command(TARGET game_2048 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:game_2048>/assets
    COMMENT "Copying assets to runtime directory"
)

# Create dist package: executable + assets in <build_dir>/dist.
# Usage: cmake --build build --target package
add_custom_target(package
    DEPENDS game_2048
    COMMAND ${CMAKE_COMMAND}
        -DEXE=$<TARGET_FILE:game_2048>
        -DDIST_DIR=${CMAKE_BINARY_DIR}/dist
        -DCMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}
        -P ${CMAKE_SOURCE_DIR}/cmake/Package.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Creating dist package"
)

# macOS: app bundle resource path
if(APPLE)
    set_target_properties(game_2048 PROPERTIES
        MACOSX_BUNDLE FALSE
    )
endif()
