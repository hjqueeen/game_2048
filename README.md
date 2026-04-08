# 2048 (C++ / SFML)

Small desktop game project for the 2048 assignment.

## Project overview

- **Type:** Desktop game
- **Language:** C++17
- **Library:** SFML 3 (Graphics, Window, System)
- **Core mechanics:** 2048 merge algorithm, random tile spawn, game-over detection, score tracking, tile animation

## Assignment requirement mapping

- **C++:** Implemented in C++17
- **Desktop application or game:** 2D desktop game (2048)
- **At least one library:** SFML is integrated through CMake
- **At least one algorithm/mechanic:** Tile merge + movement logic + random spawn + no-move detection

## Folder contents (submission)

- `src/main.cpp`: Application entry point (initialization, main loop)
- `src/game2048.hpp`, `src/game2048.cpp`: Core game domain logic
- `src/game_view.hpp`, `src/game_view.cpp`: Rendering and UI/input handling
- `CMakeLists.txt`: Build configuration and SFML linking
- `README.md`: Project documentation
- `assets/fonts/DejaVuSans-Bold.ttf`: bundled font asset used by the game
- `cmake/Package.cmake`: creates submission-ready `dist/` package
- `vcpkg.json`: optional dependency manifest (`sfml`)

## Separated architecture (file responsibilities)

- **`main` layer (`src/main.cpp`)**
  - Creates the window and initializes resources (font, game instances)
  - Runs the game loop and delegates input/rendering to dedicated modules
- **`Game2048` domain layer (`src/game2048.*`)**
  - Owns board state, score, merge algorithm, random tile spawn, and game-over checks
  - Exposes move/reset/update APIs without SFML rendering concerns
- **`GameView` presentation layer (`src/game_view.*`)**
  - Handles all visual drawing (board, tiles, score panel, game-over overlay)
  - Translates user input events (keyboard/mouse) into game actions

This separation improves maintainability, testability, and readability by isolating game rules from rendering code.

## Requirements

- CMake 3.16+
- C++17-compatible compiler
- SFML 3.0+ (Graphics, Window, System)

### Install SFML

**macOS (Homebrew)**
```bash
brew install sfml
```

**Ubuntu/Debian**
```bash
sudo apt update
sudo apt install libsfml-dev
```

**Windows**

Download SFML from the [official SFML website](https://www.sfml-dev.org/download.php) and set `SFML_DIR` or `CMAKE_PREFIX_PATH` for CMake.

## Build and run

```bash
cd game_2048
cmake -S . -B build
cmake --build build
./build/game_2048
```

### Build with vcpkg (optional)

```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_TARGET_TRIPLET=x64-osx
cmake --build build
```

## Create submission package (`dist`)

```bash
cd game_2048
cmake -S . -B build
cmake --build build --target package
```

Generated output:

- `build/dist/game_2048` (or `game_2048.exe` on Windows)
- `build/dist/assets/fonts/DejaVuSans-Bold.ttf`

## Controls

- Arrow keys: move tiles
- `R`: restart game
- New Game button (mouse): restart game

## Game rules

- Same-value tiles merge when they collide (`2 + 2 -> 4`)
- After each valid move, a new tile (`2` or `4`) spawns
- Game over when no legal move exists
- Win condition: create tile `2048`

## Third-party components (Drittleistungen)

1. **SFML 3.x**
   - Website: <https://www.sfml-dev.org/>
   - Usage: window creation, rendering, text, input events
   - License: zlib/png license
2. **DejaVu Sans Bold font**
   - File: `assets/fonts/DejaVuSans-Bold.ttf`
   - Usage: in-game text rendering
   - Upstream: DejaVu Fonts project

## Notes on portability and performance

- The game runs at 60 FPS (`window.setFramerateLimit(60)`).
- Build step copies `assets/` next to the executable automatically.
- `package` target creates `dist/` for submission.
- Tile operations are done on fixed-size arrays (`4x4`) for low overhead and predictable behavior.
