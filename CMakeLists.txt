cmake_minimum_required(VERSION 3.16)
project(game_2048 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# SFML 찾기 (시스템에 설치된 SFML 사용: macOS는 brew install sfml)
# SFML 3.x: 컴포넌트 이름 대문자, 타겟은 SFML::Graphics 등
find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)

add_executable(game_2048 src/main.cpp)
target_link_libraries(game_2048 PRIVATE SFML::Graphics SFML::Window SFML::System)

# macOS: 앱 번들 리소스 경로
if(APPLE)
    set_target_properties(game_2048 PROPERTIES
        MACOSX_BUNDLE FALSE
    )
endif()
