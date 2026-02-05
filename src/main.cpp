#include <SFML/Graphics.hpp>
#include <array>
#include <cmath>
#include <optional>
#include <random>
#include <string>
#include <tuple>
#include <vector>

constexpr int GRID_SIZE = 4;
constexpr int TILE_SIZE = 100;
constexpr int PADDING = 10;
constexpr int WINDOW_WIDTH = GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * PADDING;
constexpr int WINDOW_HEIGHT = GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * PADDING + 60;

using DisplayGrid = std::array<std::array<float, GRID_SIZE>, GRID_SIZE>;

enum class SpawnFrom { None, Left, Right, Up, Down };

class Game2048 {
public:
    using Grid = std::array<std::array<int, GRID_SIZE>, GRID_SIZE>;

    Game2048() : grid_{}, score_(0), rng_(std::random_device{}()) {
        for (int r = 0; r < GRID_SIZE; ++r)
            for (int c = 0; c < GRID_SIZE; ++c)
                displayRow_[r][c] = static_cast<float>(r), displayCol_[r][c] = static_cast<float>(c);
        spawnTile(SpawnFrom::None);
        spawnTile(SpawnFrom::None);
    }

    const Grid& getGrid() const { return grid_; }
    const DisplayGrid& getDisplayRow() const { return displayRow_; }
    const DisplayGrid& getDisplayCol() const { return displayCol_; }
    int getScore() const { return score_; }
    bool isGameOver() const { return !canMove(); }
    bool hasWon() const {
        for (const auto& row : grid_)
            for (int v : row)
                if (v == 2048) return true;
        return false;
    }

    bool moveLeft()  { return move([](int r, int c) { return std::make_pair(r, c); }, [](int i, int, float sc) { return std::make_pair(static_cast<float>(i), sc); }, SpawnFrom::Left); }
    bool moveRight() { return move([](int r, int c) { return std::make_pair(r, GRID_SIZE - 1 - c); }, [](int i, int, float sc) { return std::make_pair(static_cast<float>(i), 3.f - sc); }, SpawnFrom::Right); }
    bool moveUp()    { return move([](int r, int c) { return std::make_pair(c, r); }, [](int, int j, float sc) { return std::make_pair(sc, static_cast<float>(j)); }, SpawnFrom::Up); }
    bool moveDown()  { return move([](int r, int c) { return std::make_pair(GRID_SIZE - 1 - c, r); }, [](int, int j, float sc) { return std::make_pair(3.f - sc, static_cast<float>(j)); }, SpawnFrom::Down); }

    void reset() {
        grid_ = {};
        score_ = 0;
        for (int r = 0; r < GRID_SIZE; ++r)
            for (int c = 0; c < GRID_SIZE; ++c)
                displayRow_[r][c] = static_cast<float>(r), displayCol_[r][c] = static_cast<float>(c);
        spawnTile(SpawnFrom::None);
        spawnTile(SpawnFrom::None);
    }

private:
    Grid grid_;
    DisplayGrid displayRow_, displayCol_;
    int score_;
    std::mt19937 rng_;

    template <typename F, typename G>
    bool move(F indexer, G setDisplayFromSource, SpawnFrom spawnDir) {
        Grid prev = grid_;
        for (int r = 0; r < GRID_SIZE; ++r) {
            std::array<int, GRID_SIZE> line;
            for (int c = 0; c < GRID_SIZE; ++c) {
                auto [i, j] = indexer(r, c);
                line[c] = grid_[i][j];
            }
            auto [merged, sourceIndices, addedScore] = mergeLine(line);
            score_ += addedScore;
            for (int c = 0; c < GRID_SIZE; ++c) {
                auto [i, j] = indexer(r, c);
                grid_[i][j] = merged[c];
                auto [dr, dc] = setDisplayFromSource(i, j, sourceIndices[c]);
                displayRow_[i][j] = dr;
                displayCol_[i][j] = dc;
            }
        }
        if (grid_ != prev) {
            spawnTile(spawnDir);
            return true;
        }
        return false;
    }

    std::tuple<std::array<int, GRID_SIZE>, std::array<float, GRID_SIZE>, int> mergeLine(std::array<int, GRID_SIZE> line) {
        std::array<int, GRID_SIZE> out{};
        std::array<float, GRID_SIZE> sourceIndices{};
        int write = 0;
        int addedScore = 0;
        for (int read = 0; read < GRID_SIZE; ++read) {
            if (line[read] == 0) continue;
            if (write > 0 && out[write - 1] == line[read]) {
                out[write - 1] *= 2;
                sourceIndices[write - 1] = (sourceIndices[write - 1] + static_cast<float>(read)) / 2.f;
                addedScore += out[write - 1];
            } else {
                out[write] = line[read];
                sourceIndices[write] = static_cast<float>(read);
                ++write;
            }
        }
        return {out, sourceIndices, addedScore};
    }

    bool canMove() const {
        for (int r = 0; r < GRID_SIZE; ++r) {
            for (int c = 0; c < GRID_SIZE; ++c) {
                if (grid_[r][c] == 0) return true;
                if (c + 1 < GRID_SIZE && grid_[r][c] == grid_[r][c + 1]) return true;
                if (r + 1 < GRID_SIZE && grid_[r][c] == grid_[r + 1][c]) return true;
            }
        }
        return false;
    }

    void spawnTile(SpawnFrom from) {
        std::vector<std::pair<int, int>> candidates;
        if (from == SpawnFrom::None) {
            for (int r = 0; r < GRID_SIZE; ++r)
                for (int c = 0; c < GRID_SIZE; ++c)
                    if (grid_[r][c] == 0) candidates.emplace_back(r, c);
        } else {
            // At most 4 candidates: one per column/row (the end cell in that direction)
            if (from == SpawnFrom::Up) {
                // Up: one cell per column — topmost empty (closest to top wall)
                for (int c = 0; c < GRID_SIZE; ++c) {
                    int r = 0;
                    while (r < GRID_SIZE && grid_[r][c] != 0) ++r;
                    if (r < GRID_SIZE) candidates.emplace_back(r, c);
                }
            } else if (from == SpawnFrom::Down) {
                // Down: one cell per column — bottommost empty (closest to bottom wall)
                for (int c = 0; c < GRID_SIZE; ++c) {
                    int r = GRID_SIZE - 1;
                    while (r >= 0 && grid_[r][c] != 0) --r;
                    if (r >= 0) candidates.emplace_back(r, c);
                }
            } else if (from == SpawnFrom::Left) {
                // Left: one cell per row — leftmost empty (closest to left wall)
                for (int r = 0; r < GRID_SIZE; ++r) {
                    int c = 0;
                    while (c < GRID_SIZE && grid_[r][c] != 0) ++c;
                    if (c < GRID_SIZE) candidates.emplace_back(r, c);
                }
            } else if (from == SpawnFrom::Right) {
                // Right: one cell per row — rightmost empty (closest to right wall)
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
        grid_[r][c] = (valueDist(rng_) == 0) ? 4 : 2;
        // Set entry position so tile slides in from opposite side and moves to target
        switch (from) {
            case SpawnFrom::Left:   displayRow_[r][c] = static_cast<float>(r); displayCol_[r][c] = static_cast<float>(GRID_SIZE); break;
            case SpawnFrom::Right:  displayRow_[r][c] = static_cast<float>(r); displayCol_[r][c] = -1.f; break;
            case SpawnFrom::Up:     displayRow_[r][c] = static_cast<float>(GRID_SIZE); displayCol_[r][c] = static_cast<float>(c); break;
            case SpawnFrom::Down:   displayRow_[r][c] = -1.f; displayCol_[r][c] = static_cast<float>(c); break;
            case SpawnFrom::None:   displayRow_[r][c] = static_cast<float>(r); displayCol_[r][c] = static_cast<float>(c); break;
        }
    }

public:
    void updateAnimation(float dt) {
        const float speed = 12.f;
        for (int r = 0; r < GRID_SIZE; ++r) {
            for (int c = 0; c < GRID_SIZE; ++c) {
                float tr = static_cast<float>(r), tc = static_cast<float>(c);
                float dr = tr - displayRow_[r][c], dc = tc - displayCol_[r][c];
                float t = std::min(1.f, dt * speed);
                displayRow_[r][c] += dr * t;
                displayCol_[r][c] += dc * t;
            }
        }
    }
};

sf::Color getTileColor(int value) {
    switch (value) {
        case 0:     return sf::Color(205, 193, 180);
        case 2:     return sf::Color(238, 228, 218);
        case 4:     return sf::Color(237, 224, 200);
        case 8:     return sf::Color(242, 177, 121);
        case 16:    return sf::Color(245, 149, 99);
        case 32:    return sf::Color(246, 124, 95);
        case 64:    return sf::Color(246, 94, 59);
        case 128:   return sf::Color(237, 207, 114);
        case 256:   return sf::Color(237, 204, 97);
        case 512:   return sf::Color(237, 200, 80);
        case 1024:  return sf::Color(237, 197, 63);
        case 2048:  return sf::Color(237, 194, 46);
        default:    return sf::Color(60, 58, 50);
    }
}

sf::Color getTextColor(int value) {
    return (value <= 4) ? sf::Color(119, 110, 101) : sf::Color(249, 246, 242);
}

int main() {
    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "2048", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    Game2048 game;

    sf::Font font;
    bool fontLoaded = font.openFromFile("/System/Library/Fonts/Helvetica.ttc")
        || font.openFromFile("/System/Library/Fonts/SFNSText.ttf")
        || font.openFromFile("/Library/Fonts/Arial.ttf")
        || font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");

    sf::RectangleShape bgShape(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    bgShape.setFillColor(sf::Color(250, 248, 239));

    sf::RectangleShape gridBg(sf::Vector2f(GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * PADDING,
                                           GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * PADDING));
    // 그리드 전체를 조금 아래로 내려서 상단에 스코어/버튼 공간 확보
    gridBg.setPosition({PADDING, PADDING + 70});
    gridBg.setFillColor(sf::Color(187, 173, 160));
    gridBg.setOutlineThickness(0);

    // Fixed background: empty cell grid (always same position)
    std::vector<sf::RectangleShape> cellBackgrounds;
    for (int r = 0; r < GRID_SIZE; ++r) {
        for (int c = 0; c < GRID_SIZE; ++c) {
            sf::RectangleShape cell(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            cell.setPosition({PADDING + (c + 1) * PADDING + c * TILE_SIZE + 0.f,
                              PADDING + 70 + (r + 1) * PADDING + r * TILE_SIZE + 0.f});
            cell.setFillColor(sf::Color(205, 193, 180));
            cell.setOutlineThickness(0);
            cellBackgrounds.push_back(cell);
        }
    }
    // Moving number tiles (drawn at display position)
    std::vector<sf::RectangleShape> tileShapes;
    std::vector<sf::Text> tileTexts;
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; ++i) {
        tileShapes.push_back(sf::RectangleShape(sf::Vector2f(TILE_SIZE, TILE_SIZE)));
        tileShapes.back().setOutlineThickness(0);
        tileTexts.push_back(sf::Text(font, "", 36));
        tileTexts.back().setFillColor(sf::Color(119, 110, 101));
    }

    // ----- 상단 스코어 / 베스트 패널 -----
    const float panelWidth  = 96.f;
    const float panelHeight = 56.f;
    const float panelTop    = 8.f;
    const float panelGap    = 12.f;

    sf::RectangleShape scorePanel(sf::Vector2f(panelWidth, panelHeight));
    sf::RectangleShape bestPanel(sf::Vector2f(panelWidth, panelHeight));

    scorePanel.setFillColor(sf::Color(187, 173, 160));
    bestPanel.setFillColor(sf::Color(187, 173, 160));

    scorePanel.setPosition({
        WINDOW_WIDTH / 2.f - panelWidth - panelGap / 2.f,
        panelTop
    });
    bestPanel.setPosition({
        WINDOW_WIDTH / 2.f + panelGap / 2.f,
        panelTop
    });

    sf::Text scoreLabel(font, "SCORE", 14);
    sf::Text scoreValue(font, "0", 22);
    sf::Text bestLabel(font, "BEST", 14);
    sf::Text bestValue(font, "0", 22);

    scoreLabel.setFillColor(sf::Color(238, 228, 218));
    bestLabel.setFillColor(sf::Color(238, 228, 218));
    scoreValue.setFillColor(sf::Color(249, 246, 242));
    bestValue.setFillColor(sf::Color(249, 246, 242));

    sf::Text gameOverText(font, "Game Over! (R to restart)", 28);
    gameOverText.setPosition({WINDOW_WIDTH / 2.f - 140, WINDOW_HEIGHT / 2.f - 20});
    gameOverText.setFillColor(sf::Color(119, 110, 101));

    // ----- 새 게임 버튼 -----
    const float buttonWidth  = 120.f;
    const float buttonHeight = 48.f;
    sf::RectangleShape newGameButton(sf::Vector2f(buttonWidth, buttonHeight));
    newGameButton.setFillColor(sf::Color(143, 122, 102));
    newGameButton.setPosition({
        WINDOW_WIDTH - buttonWidth - PADDING,
        panelTop + (panelHeight - buttonHeight) / 2.f
    });

    sf::Text newGameText(font, "New Game", 20);
    newGameText.setFillColor(sf::Color(249, 246, 242));
    {
        sf::FloatRect b = newGameText.getLocalBounds();
        newGameText.setPosition({
            newGameButton.getPosition().x + (buttonWidth - b.size.x) / 2.f - b.position.x,
            newGameButton.getPosition().y + (buttonHeight - b.size.y) / 2.f - b.position.y - 2.f
        });
    }

    bool gameOver = false;
    int bestScore = 0;
    sf::Clock frameClock;

    while (window.isOpen()) {
        float dt = frameClock.restart().asSeconds();
        game.updateAnimation(dt);

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::R) {
                    game.reset();
                    gameOver = false;
                }
                if (!gameOver) {
                    if (keyPressed->code == sf::Keyboard::Key::Left)  game.moveLeft();
                    if (keyPressed->code == sf::Keyboard::Key::Right) game.moveRight();
                    if (keyPressed->code == sf::Keyboard::Key::Up)    game.moveUp();
                    if (keyPressed->code == sf::Keyboard::Key::Down)  game.moveDown();
                    if (game.isGameOver()) gameOver = true;
                }
            }
            else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos(static_cast<float>(mousePressed->position.x),
                                          static_cast<float>(mousePressed->position.y));
                    if (newGameButton.getGlobalBounds().contains(mousePos)) {
                        game.reset();
                        gameOver = false;
                    }
                }
            }
        }

        const auto& grid = game.getGrid();
        const auto& displayRow = game.getDisplayRow();
        const auto& displayCol = game.getDisplayCol();
        for (int r = 0; r < GRID_SIZE; ++r) {
            for (int c = 0; c < GRID_SIZE; ++c) {
                int idx = r * GRID_SIZE + c;
                int value = grid[r][c];
                if (value == 0) continue;
                float dr = displayRow[r][c], dc = displayCol[r][c];
                float px = PADDING + (dc + 1) * PADDING + dc * TILE_SIZE;
                float py = PADDING + 70 + (dr + 1) * PADDING + dr * TILE_SIZE;
                tileShapes[idx].setPosition({px, py});
                tileShapes[idx].setFillColor(getTileColor(value));
                tileTexts[idx].setString(std::to_string(value));
                tileTexts[idx].setFillColor(getTextColor(value));
                tileTexts[idx].setCharacterSize(value >= 1024 ? 30 : (value >= 128 ? 36 : 42));
                sf::FloatRect bounds = tileTexts[idx].getLocalBounds();
                tileTexts[idx].setPosition({
                    px + (TILE_SIZE - bounds.size.x) / 2.f - bounds.position.x,
                    py + (TILE_SIZE - bounds.size.y) / 2.f - bounds.position.y - 4
                });
            }
        }

        // 스코어 / 베스트 값 갱신
        bestScore = std::max(bestScore, game.getScore());
        scoreValue.setString(std::to_string(game.getScore()));
        bestValue.setString(std::to_string(bestScore));

        // 각 패널 안에서 텍스트를 가운데 정렬
        auto centerTextInPanel = [](sf::Text& label, sf::Text& value, const sf::RectangleShape& panel) {
            sf::FloatRect lb = label.getLocalBounds();
            sf::FloatRect vb = value.getLocalBounds();
            float px = panel.getPosition().x;
            float py = panel.getPosition().y;
            float pw = panel.getSize().x;
            float ph = panel.getSize().y;

            label.setPosition({
                px + (pw - lb.size.x) / 2.f - lb.position.x,
                py + 6.f - lb.position.y
            });
            value.setPosition({
                px + (pw - vb.size.x) / 2.f - vb.position.x,
                py + ph / 2.f - vb.size.y / 2.f - vb.position.y + 6.f
            });
        };

        centerTextInPanel(scoreLabel, scoreValue, scorePanel);
        centerTextInPanel(bestLabel, bestValue, bestPanel);

        window.clear(sf::Color(250, 248, 239));
        window.draw(bgShape);
        window.draw(scorePanel);
        window.draw(bestPanel);
        window.draw(newGameButton);
        window.draw(gridBg);
        for (const auto& t : cellBackgrounds) window.draw(t);
        for (int r = 0; r < GRID_SIZE; ++r)
            for (int c = 0; c < GRID_SIZE; ++c)
                if (grid[r][c] != 0) {
                    window.draw(tileShapes[r * GRID_SIZE + c]);
                    if (fontLoaded) window.draw(tileTexts[r * GRID_SIZE + c]);
                }
        if (fontLoaded) {
            window.draw(scoreLabel);
            window.draw(scoreValue);
            window.draw(bestLabel);
            window.draw(bestValue);
            window.draw(newGameText);
            if (gameOver) window.draw(gameOverText);
        }
        window.display();
    }
    return 0;
}