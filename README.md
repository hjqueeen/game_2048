# 2048 (C++ / SFML)

A 2048 game built with C++ and SFML.

## Requirements

- **CMake** 3.16 or higher
- **C++17** compatible compiler
- **SFML 3.0** or higher (Graphics, Window, System)

### Installing SFML

**macOS (Homebrew):**
```bash
brew install sfml
```

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install libsfml-dev
```

**Windows:**  
Download from [SFML official site](https://www.sfml-dev.org/download.php), then set `SFML_DIR` or `CMAKE_PREFIX_PATH` in CMake.

## Build and run

```bash
cd game_2048
cmake -B build
cmake --build build
./build/game_2048
```

## Controls

- **Arrow keys**: Move tiles (up / down / left / right)
- **R**: Restart game (after game over)

## Rules

- Tiles with the same number merge when they touch (e.g. 2 + 2 → 4).
- After each move, a new tile (2 or 4) appears in an empty cell.
- Game over when no move is possible.
- Reaching a 2048 tile is a win (current version allows restart only after game over).
