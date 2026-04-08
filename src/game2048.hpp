#pragma once

#include <array>
#include <random>
#include <tuple>

constexpr int GRID_SIZE = 4;
using DisplayGrid = std::array<std::array<float, GRID_SIZE>, GRID_SIZE>;

class Game2048 {
public:
    using Grid = std::array<std::array<int, GRID_SIZE>, GRID_SIZE>;

    Game2048();

    const Grid& getGrid() const;
    const DisplayGrid& getDisplayRow() const;
    const DisplayGrid& getDisplayCol() const;
    int getScore() const;
    bool isGameOver() const;
    bool hasWon() const;

    bool moveLeft();
    bool moveRight();
    bool moveUp();
    bool moveDown();

    void reset();
    void updateAnimation(float dt);

private:
    enum class SpawnFrom { None, Left, Right, Up, Down };

    template <typename F, typename G>
    bool move(F indexer, G setDisplayFromSource, SpawnFrom spawnDir);

    std::tuple<std::array<int, GRID_SIZE>, std::array<float, GRID_SIZE>, int> mergeLine(
        std::array<int, GRID_SIZE> line);
    bool canMove() const;
    void spawnTile(SpawnFrom from);

    Grid grid_;
    DisplayGrid displayRow_{};
    DisplayGrid displayCol_{};
    int score_ = 0;
    std::mt19937 rng_;
};
