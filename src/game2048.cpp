#include "game2048.hpp"

#include <algorithm>
#include <utility>
#include <vector>

Game2048::Game2048() : grid_{}, rng_(std::random_device{}()) {
    // Initialize display coordinates to exact grid coordinates.
    for (int r = 0; r < GRID_SIZE; ++r) {
        for (int c = 0; c < GRID_SIZE; ++c) {
            displayRow_[r][c] = static_cast<float>(r);
            displayCol_[r][c] = static_cast<float>(c);
        }
    }
    // Start with two random tiles, like classic 2048.
    spawnTile(SpawnFrom::None);
    spawnTile(SpawnFrom::None);
}

const Game2048::Grid& Game2048::getGrid() const { return grid_; }
const DisplayGrid& Game2048::getDisplayRow() const { return displayRow_; }
const DisplayGrid& Game2048::getDisplayCol() const { return displayCol_; }
int Game2048::getScore() const { return score_; }
bool Game2048::isGameOver() const { return !canMove(); }

bool Game2048::hasWon() const {
    for (const auto& row : grid_) {
        for (int v : row) {
            if (v == 2048) return true;
        }
    }
    return false;
}

bool Game2048::moveLeft() {
    return move(
        [](int r, int c) { return std::make_pair(r, c); },
        [](int i, int, float sc) { return std::make_pair(static_cast<float>(i), sc); },
        SpawnFrom::Left);
}

bool Game2048::moveRight() {
    return move(
        [](int r, int c) { return std::make_pair(r, GRID_SIZE - 1 - c); },
        [](int i, int, float sc) { return std::make_pair(static_cast<float>(i), 3.f - sc); },
        SpawnFrom::Right);
}

bool Game2048::moveUp() {
    return move(
        [](int r, int c) { return std::make_pair(c, r); },
        [](int, int j, float sc) { return std::make_pair(sc, static_cast<float>(j)); },
        SpawnFrom::Up);
}

bool Game2048::moveDown() {
    return move(
        [](int r, int c) { return std::make_pair(GRID_SIZE - 1 - c, r); },
        [](int, int j, float sc) { return std::make_pair(3.f - sc, static_cast<float>(j)); },
        SpawnFrom::Down);
}

void Game2048::reset() {
    grid_ = {};
    score_ = 0;
    for (int r = 0; r < GRID_SIZE; ++r) {
        for (int c = 0; c < GRID_SIZE; ++c) {
            displayRow_[r][c] = static_cast<float>(r);
            displayCol_[r][c] = static_cast<float>(c);
        }
    }
    spawnTile(SpawnFrom::None);
    spawnTile(SpawnFrom::None);
}

template <typename F, typename G>
bool Game2048::move(F indexer, G setDisplayFromSource, SpawnFrom spawnDir) {
    // Snapshot current grid to detect whether the move changed state.
    Grid prev = grid_;
    for (int r = 0; r < GRID_SIZE; ++r) {
        std::array<int, GRID_SIZE> line;
        for (int c = 0; c < GRID_SIZE; ++c) {
            auto [i, j] = indexer(r, c);
            line[c] = grid_[i][j];
        }
        // Merge one logical line and collect score gained by this merge.
        auto [merged, sourceIndices, addedScore] = mergeLine(line);
        score_ += addedScore;
        for (int c = 0; c < GRID_SIZE; ++c) {
            auto [i, j] = indexer(r, c);
            grid_[i][j] = merged[c];
            // Keep source positions for smooth tile movement animation.
            auto [dr, dc] = setDisplayFromSource(i, j, sourceIndices[c]);
            displayRow_[i][j] = dr;
            displayCol_[i][j] = dc;
        }
    }
    if (grid_ != prev) {
        // Spawn a new tile only after a valid state-changing move.
        spawnTile(spawnDir);
        return true;
    }
    return false;
}

std::tuple<std::array<int, GRID_SIZE>, std::array<float, GRID_SIZE>, int> Game2048::mergeLine(
    std::array<int, GRID_SIZE> line) {
    std::array<int, GRID_SIZE> out{};
    std::array<float, GRID_SIZE> sourceIndices{};
    int write = 0;
    int addedScore = 0;
    // Compress and merge from left to right in one pass.
    for (int read = 0; read < GRID_SIZE; ++read) {
        if (line[read] == 0) continue;
        if (write > 0 && out[write - 1] == line[read]) {
            // Same value merges into one doubled tile.
            out[write - 1] *= 2;
            sourceIndices[write - 1] =
                (sourceIndices[write - 1] + static_cast<float>(read)) / 2.f;
            addedScore += out[write - 1];
        } else {
            out[write] = line[read];
            sourceIndices[write] = static_cast<float>(read);
            ++write;
        }
    }
    return {out, sourceIndices, addedScore};
}

bool Game2048::canMove() const {
    // Move is possible if at least one empty cell exists or a neighbor can merge.
    for (int r = 0; r < GRID_SIZE; ++r) {
        for (int c = 0; c < GRID_SIZE; ++c) {
            if (grid_[r][c] == 0) return true;
            if (c + 1 < GRID_SIZE && grid_[r][c] == grid_[r][c + 1]) return true;
            if (r + 1 < GRID_SIZE && grid_[r][c] == grid_[r + 1][c]) return true;
        }
    }
    return false;
}

void Game2048::spawnTile(SpawnFrom from) {
    std::vector<std::pair<int, int>> candidates;
    if (from == SpawnFrom::None) {
        // Initial spawn/reset: any empty position is allowed.
        for (int r = 0; r < GRID_SIZE; ++r) {
            for (int c = 0; c < GRID_SIZE; ++c) {
                if (grid_[r][c] == 0) candidates.emplace_back(r, c);
            }
        }
    } else {
        // Directional spawn: choose edge-side candidates for a stronger "flow" feeling.
        if (from == SpawnFrom::Up) {
            for (int c = 0; c < GRID_SIZE; ++c) {
                int r = 0;
                while (r < GRID_SIZE && grid_[r][c] != 0) ++r;
                if (r < GRID_SIZE) candidates.emplace_back(r, c);
            }
        } else if (from == SpawnFrom::Down) {
            for (int c = 0; c < GRID_SIZE; ++c) {
                int r = GRID_SIZE - 1;
                while (r >= 0 && grid_[r][c] != 0) --r;
                if (r >= 0) candidates.emplace_back(r, c);
            }
        } else if (from == SpawnFrom::Left) {
            for (int r = 0; r < GRID_SIZE; ++r) {
                int c = 0;
                while (c < GRID_SIZE && grid_[r][c] != 0) ++c;
                if (c < GRID_SIZE) candidates.emplace_back(r, c);
            }
        } else if (from == SpawnFrom::Right) {
            for (int r = 0; r < GRID_SIZE; ++r) {
                int c = GRID_SIZE - 1;
                while (c >= 0 && grid_[r][c] != 0) --c;
                if (c >= 0) candidates.emplace_back(r, c);
            }
        }
    }

    if (candidates.empty()) return;
    std::uniform_int_distribution<int> idxDist(0, static_cast<int>(candidates.size()) - 1);
    auto [r, c] = candidates[idxDist(rng_)];
    std::uniform_int_distribution<int> valueDist(0, 9);
    // 90% chance for 2, 10% chance for 4.
    grid_[r][c] = (valueDist(rng_) == 0) ? 4 : 2;

    // Place tile just outside board so it slides into the target cell.
    switch (from) {
        case SpawnFrom::Left:
            displayRow_[r][c] = static_cast<float>(r);
            displayCol_[r][c] = static_cast<float>(GRID_SIZE);
            break;
        case SpawnFrom::Right:
            displayRow_[r][c] = static_cast<float>(r);
            displayCol_[r][c] = -1.f;
            break;
        case SpawnFrom::Up:
            displayRow_[r][c] = static_cast<float>(GRID_SIZE);
            displayCol_[r][c] = static_cast<float>(c);
            break;
        case SpawnFrom::Down:
            displayRow_[r][c] = -1.f;
            displayCol_[r][c] = static_cast<float>(c);
            break;
        case SpawnFrom::None:
            displayRow_[r][c] = static_cast<float>(r);
            displayCol_[r][c] = static_cast<float>(c);
            break;
    }
}

void Game2048::updateAnimation(float dt) {
    const float speed = 12.f;
    // Exponential-like interpolation toward final grid coordinates.
    for (int r = 0; r < GRID_SIZE; ++r) {
        for (int c = 0; c < GRID_SIZE; ++c) {
            float tr = static_cast<float>(r);
            float tc = static_cast<float>(c);
            float dr = tr - displayRow_[r][c];
            float dc = tc - displayCol_[r][c];
            float t = std::min(1.f, dt * speed);
            displayRow_[r][c] += dr * t;
            displayCol_[r][c] += dc * t;
        }
    }
}
