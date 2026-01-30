#include <SFML/Graphics.hpp>
#include <array>
#include <optional>
#include <random>
#include <string>
#include <vector>

constexpr int GRID_SIZE = 4;
constexpr int TILE_SIZE = 100;
constexpr int PADDING = 10;
constexpr int WINDOW_WIDTH = GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * PADDING;
constexpr int WINDOW_HEIGHT = GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * PADDING + 60;

class Game2048 {
public:
    using Grid = std::array<std::array<int, GRID_SIZE>, GRID_SIZE>;

    Game2048() : grid_{}, score_(0), rng_(std::random_device{}()) {
        spawnTile();
        spawnTile();
    }

    const Grid& getGrid() const { return grid_; }
    int getScore() const { return score_; }
    bool isGameOver() const { return !canMove(); }
    bool hasWon() const {
        for (const auto& row : grid_)
            for (int v : row)
                if (v == 2048) return true;
        return false;
    }

    bool moveLeft()  { return move([](int r, int c) { return std::make_pair(r, c); }); }
    bool moveRight() { return move([](int r, int c) { return std::make_pair(r, GRID_SIZE - 1 - c); }); }
    bool moveUp()    { return move([](int r, int c) { return std::make_pair(c, r); }); }
    bool moveDown()  { return move([](int r, int c) { return std::make_pair(GRID_SIZE - 1 - c, r); }); }

    void reset() {
        grid_ = {};
        score_ = 0;
        spawnTile();
        spawnTile();
    }

private:
    Grid grid_;
    int score_;
    std::mt19937 rng_;

    static int indexAt(int row, int col) { return row * GRID_SIZE + col; }
    static std::pair<int, int> posAt(int idx) { return {idx / GRID_SIZE, idx % GRID_SIZE}; }

    bool move(auto indexer) {
        Grid prev = grid_;
        for (int r = 0; r < GRID_SIZE; ++r) {
            std::array<int, GRID_SIZE> line;
            for (int c = 0; c < GRID_SIZE; ++c) {
                auto [i, j] = indexer(r, c);
                line[c] = grid_[i][j];
            }
            auto [merged, addedScore] = mergeLine(line);
            score_ += addedScore;
            for (int c = 0; c < GRID_SIZE; ++c) {
                auto [i, j] = indexer(r, c);
                grid_[i][j] = merged[c];
            }
        }
        if (grid_ != prev) {
            spawnTile();
            return true;
        }
        return false;
    }

    std::pair<std::array<int, GRID_SIZE>, int> mergeLine(std::array<int, GRID_SIZE> line) {
        std::array<int, GRID_SIZE> out{};
        int write = 0;
        int addedScore = 0;
        for (int read = 0; read < GRID_SIZE; ++read) {
            if (line[read] == 0) continue;
            if (write > 0 && out[write - 1] == line[read]) {
                out[write - 1] *= 2;
                addedScore += out[write - 1];
            } else {
                out[write++] = line[read];
            }
        }
        return {out, addedScore};
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

    void spawnTile() {
        std::vector<std::pair<int, int>> empty;
        for (int r = 0; r < GRID_SIZE; ++r)
            for (int c = 0; c < GRID_SIZE; ++c)
                if (grid_[r][c] == 0) empty.emplace_back(r, c);
        if (empty.empty()) return;
        std::uniform_int_distribution<int> idxDist(0, static_cast<int>(empty.size()) - 1);
        auto [r, c] = empty[idxDist(rng_)];
        std::uniform_int_distribution<int> valueDist(0, 9);
        grid_[r][c] = (valueDist(rng_) == 0) ? 4 : 2;
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
    gridBg.setPosition({PADDING, PADDING + 50});
    gridBg.setFillColor(sf::Color(187, 173, 160));
    gridBg.setOutlineThickness(0);

    std::vector<sf::RectangleShape> tileShapes;
    std::vector<sf::Text> tileTexts;
    for (int r = 0; r < GRID_SIZE; ++r) {
        for (int c = 0; c < GRID_SIZE; ++c) {
            sf::RectangleShape tile(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            tile.setPosition({PADDING + (c + 1) * PADDING + c * TILE_SIZE + 0.f,
                             PADDING + 50 + (r + 1) * PADDING + r * TILE_SIZE + 0.f});
            tile.setFillColor(sf::Color(205, 193, 180));
            tile.setOutlineThickness(0);
            tileShapes.push_back(tile);

            sf::Text text(font, "", 36);
            text.setFillColor(sf::Color(119, 110, 101));
            tileTexts.push_back(text);
        }
    }

    sf::Text scoreText(font, "Score: 0", 24);
    scoreText.setPosition({PADDING, 8});
    scoreText.setFillColor(sf::Color(119, 110, 101));

    sf::Text gameOverText(font, "Game Over! (R to restart)", 28);
    gameOverText.setPosition({WINDOW_WIDTH / 2.f - 140, WINDOW_HEIGHT / 2.f - 20});
    gameOverText.setFillColor(sf::Color(119, 110, 101));

    bool gameOver = false;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->key == sf::Keyboard::Key::R) {
                    game.reset();
                    gameOver = false;
                }
                if (!gameOver) {
                    if (keyPressed->key == sf::Keyboard::Key::Left)  game.moveLeft();
                    if (keyPressed->key == sf::Keyboard::Key::Right) game.moveRight();
                    if (keyPressed->key == sf::Keyboard::Key::Up)    game.moveUp();
                    if (keyPressed->key == sf::Keyboard::Key::Down)  game.moveDown();
                    if (game.isGameOver()) gameOver = true;
                }
            }
        }

        const auto& grid = game.getGrid();
        for (int r = 0; r < GRID_SIZE; ++r) {
            for (int c = 0; c < GRID_SIZE; ++c) {
                int idx = r * GRID_SIZE + c;
                int value = grid[r][c];
                tileShapes[idx].setFillColor(getTileColor(value));
                if (value != 0) {
                    tileTexts[idx].setString(std::to_string(value));
                    tileTexts[idx].setFillColor(getTextColor(value));
                    tileTexts[idx].setCharacterSize(value >= 1024 ? 30 : (value >= 128 ? 36 : 42));
                    sf::FloatRect bounds = tileTexts[idx].getLocalBounds();
                    tileTexts[idx].setPosition({
                        tileShapes[idx].getPosition().x + (TILE_SIZE - bounds.size.x) / 2.f - bounds.position.x,
                        tileShapes[idx].getPosition().y + (TILE_SIZE - bounds.size.y) / 2.f - bounds.position.y - 4
                    });
                } else {
                    tileTexts[idx].setString("");
                }
            }
        }
        scoreText.setString("Score: " + std::to_string(game.getScore()));

        window.clear(sf::Color(250, 248, 239));
        window.draw(bgShape);
        window.draw(gridBg);
        for (const auto& t : tileShapes) window.draw(t);
        if (fontLoaded) {
            for (const auto& t : tileTexts) window.draw(t);
            window.draw(scoreText);
            if (gameOver) window.draw(gameOverText);
        }
        window.display();
    }
    return 0;
}
</think>
merge 시 점수를 반영하도록 Game 클래스 수정 중입니다.
<｜tool▁calls▁begin｜><｜tool▁call▁begin｜>
StrReplace