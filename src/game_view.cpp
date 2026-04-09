#include "game_view.hpp"

#include <algorithm>
#include <string>

namespace {

sf::Color getTileColor(int value) {
    switch (value) {
        case 0: return sf::Color(205, 193, 180);
        case 2: return sf::Color(238, 228, 218);
        case 4: return sf::Color(237, 224, 200);
        case 8: return sf::Color(242, 177, 121);
        case 16: return sf::Color(245, 149, 99);
        case 32: return sf::Color(246, 124, 95);
        case 64: return sf::Color(246, 94, 59);
        case 128: return sf::Color(237, 207, 114);
        case 256: return sf::Color(237, 204, 97);
        case 512: return sf::Color(237, 200, 80);
        case 1024: return sf::Color(237, 197, 63);
        case 2048: return sf::Color(237, 194, 46);
        default: return sf::Color(60, 58, 50);
    }
}

sf::Color getTextColor(int value) {
    return (value <= 4) ? sf::Color(119, 110, 101) : sf::Color(249, 246, 242);
}

}  // namespace

GameView::GameView(const sf::Font& font)
    : font_(font),
      scoreLabel_(font_, "SCORE", 14),
      scoreValue_(font_, "0", 22),
      bestLabel_(font_, "BEST", 14),
      bestValue_(font_, "0", 22),
      gameOverText_(font_, "Game Over! (R to restart)", 28),
      newGameText_(font_, "New Game", 20) {
    // Static background layers.
    bgShape_ = sf::RectangleShape(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    bgShape_.setFillColor(sf::Color(250, 248, 239));

    gridBg_ = sf::RectangleShape(sf::Vector2f(
        GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * PADDING,
        GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * PADDING));
    gridBg_.setPosition({PADDING, PADDING + 70});
    gridBg_.setFillColor(sf::Color(187, 173, 160));
    gridBg_.setOutlineThickness(0);

    // Prebuild fixed cell slots (the board holes).
    for (int r = 0; r < GRID_SIZE; ++r) {
        for (int c = 0; c < GRID_SIZE; ++c) {
            sf::RectangleShape cell(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            cell.setPosition({PADDING + (c + 1) * PADDING + c * TILE_SIZE + 0.f,
                              PADDING + 70 + (r + 1) * PADDING + r * TILE_SIZE + 0.f});
            cell.setFillColor(sf::Color(205, 193, 180));
            cell.setOutlineThickness(0);
            cellBackgrounds_.push_back(cell);
        }
    }

    // Reusable tile objects to avoid per-frame allocations.
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; ++i) {
        tileShapes_.push_back(sf::RectangleShape(sf::Vector2f(TILE_SIZE, TILE_SIZE)));
        tileShapes_.back().setOutlineThickness(0);
        tileTexts_.push_back(sf::Text(font_, "", 36));
        tileTexts_.back().setFillColor(sf::Color(119, 110, 101));
    }

    scoreLabel_.setFillColor(sf::Color(238, 228, 218));
    scoreValue_.setFillColor(sf::Color(249, 246, 242));
    bestLabel_.setFillColor(sf::Color(119, 110, 101));
    bestValue_.setFillColor(sf::Color(119, 110, 101));
    gameOverText_.setFillColor(sf::Color(119, 110, 101));

    const float buttonWidth = 120.f;
    const float buttonHeight = 48.f;
    newGameButton_ = sf::RectangleShape({buttonWidth, buttonHeight});
    newGameButton_.setFillColor(sf::Color(143, 122, 102));
    newGameButton_.setPosition({
        WINDOW_WIDTH - buttonWidth - PADDING,
        8.f
    });

    newGameText_.setFillColor(sf::Color(249, 246, 242));
    sf::FloatRect b = newGameText_.getLocalBounds();
    newGameText_.setPosition({
        newGameButton_.getPosition().x + (buttonWidth - b.size.x) / 2.f - b.position.x,
        newGameButton_.getPosition().y + (buttonHeight - b.size.y) / 2.f - b.position.y - 2.f
    });
}

bool GameView::processEvent(const sf::Event& event, Game2048& game, bool& gameOver) {
    bool didMove = false;

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        // Global restart hotkey.
        if (keyPressed->code == sf::Keyboard::Key::R ||
            keyPressed->scancode == sf::Keyboard::Scancode::R) {
            game.reset();
            gameOver = false;
            return true;
        }

        if (!gameOver) {
            // Map arrow keys to game moves.
            if (keyPressed->code == sf::Keyboard::Key::Left) didMove = game.moveLeft() || didMove;
            if (keyPressed->code == sf::Keyboard::Key::Right) didMove = game.moveRight() || didMove;
            if (keyPressed->code == sf::Keyboard::Key::Up) didMove = game.moveUp() || didMove;
            if (keyPressed->code == sf::Keyboard::Key::Down) didMove = game.moveDown() || didMove;
            if (game.isGameOver()) gameOver = true;
        }
    } else if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        // Restart via UI button click.
        if (mousePressed->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos(static_cast<float>(mousePressed->position.x),
                                  static_cast<float>(mousePressed->position.y));
            if (newGameButton_.getGlobalBounds().contains(mousePos)) {
                game.reset();
                gameOver = false;
                return true;
            }
        }
    }

    return didMove;
}

void GameView::draw(sf::RenderWindow& window, const Game2048& game, bool gameOver, int bestScore) {
    const auto& grid = game.getGrid();
    const auto& displayRow = game.getDisplayRow();
    const auto& displayCol = game.getDisplayCol();

    // Update visual state of active tiles from logical board + animation coordinates.
    for (int r = 0; r < GRID_SIZE; ++r) {
        for (int c = 0; c < GRID_SIZE; ++c) {
            int idx = r * GRID_SIZE + c;
            int value = grid[r][c];
            if (value == 0) continue;
            float dr = displayRow[r][c];
            float dc = displayCol[r][c];
            float px = PADDING + (dc + 1) * PADDING + dc * TILE_SIZE;
            float py = PADDING + 70 + (dr + 1) * PADDING + dr * TILE_SIZE;
            tileShapes_[idx].setPosition({px, py});
            tileShapes_[idx].setFillColor(getTileColor(value));
            tileTexts_[idx].setString(std::to_string(value));
            tileTexts_[idx].setFillColor(getTextColor(value));
            tileTexts_[idx].setCharacterSize(value >= 1024 ? 30 : (value >= 128 ? 36 : 42));
            sf::FloatRect bounds = tileTexts_[idx].getLocalBounds();
            tileTexts_[idx].setPosition({
                px + (TILE_SIZE - bounds.size.x) / 2.f - bounds.position.x,
                py + (TILE_SIZE - bounds.size.y) / 2.f - bounds.position.y - 4
            });
        }
    }

    scoreValue_.setString(std::to_string(game.getScore()));
    bestValue_.setString(std::to_string(bestScore));

    // Helper for centering score labels and values inside their panels.
    auto centerText = [](sf::Text& label, sf::Text& value, float px, float py, float pw, float ph) {
        sf::FloatRect lb = label.getLocalBounds();
        sf::FloatRect vb = value.getLocalBounds();
        label.setPosition({px + (pw - lb.size.x) / 2.f - lb.position.x, py + 6.f - lb.position.y});
        value.setPosition({
            px + (pw - vb.size.x) / 2.f - vb.position.x,
            py + ph / 2.f - vb.size.y / 2.f - vb.position.y + 6.f
        });
    };

    const float panelWidth = 96.f;
    const float panelHeight = 56.f;
    const float panelTop = 8.f;
    const float panelGap = 12.f;

    sf::RectangleShape scorePanel({panelWidth, panelHeight});
    scorePanel.setFillColor(sf::Color(187, 173, 160));
    scorePanel.setPosition({PADDING, panelTop});

    sf::RectangleShape bestPanel({panelWidth, panelHeight});
    bestPanel.setFillColor(sf::Color(250, 248, 239));
    bestPanel.setOutlineThickness(2.f);
    bestPanel.setOutlineColor(sf::Color(187, 173, 160));
    bestPanel.setPosition({PADDING + panelWidth + panelGap, panelTop});

    centerText(scoreLabel_, scoreValue_, scorePanel.getPosition().x, scorePanel.getPosition().y, panelWidth, panelHeight);
    centerText(bestLabel_, bestValue_, bestPanel.getPosition().x, bestPanel.getPosition().y, panelWidth, panelHeight);

    // Render pipeline: background -> board -> tiles -> overlays.
    window.clear(sf::Color(250, 248, 239));
    window.draw(bgShape_);
    window.draw(scorePanel);
    window.draw(bestPanel);
    window.draw(newGameButton_);
    window.draw(gridBg_);

    for (const auto& t : cellBackgrounds_) window.draw(t);
    for (int r = 0; r < GRID_SIZE; ++r) {
        for (int c = 0; c < GRID_SIZE; ++c) {
            if (grid[r][c] != 0) {
                window.draw(tileShapes_[r * GRID_SIZE + c]);
                window.draw(tileTexts_[r * GRID_SIZE + c]);
            }
        }
    }

    window.draw(scoreLabel_);
    window.draw(scoreValue_);
    window.draw(bestLabel_);
    window.draw(bestValue_);
    window.draw(newGameText_);

    if (gameOver) {
        // Draw semi-transparent game-over overlay in the center.
        sf::FloatRect textBounds = gameOverText_.getLocalBounds();
        float paddingX = 20.f;
        float paddingY = 12.f;
        float bgWidth = textBounds.size.x + paddingX * 2.f;
        float bgHeight = textBounds.size.y + paddingY * 2.f;
        gameOverBg_.setSize({bgWidth, bgHeight});
        gameOverBg_.setFillColor(sf::Color(238, 228, 218, 220));

        float bgX = WINDOW_WIDTH / 2.f - bgWidth / 2.f;
        float bgY = WINDOW_HEIGHT / 2.f - bgHeight / 2.f;
        gameOverBg_.setPosition({bgX, bgY});
        gameOverText_.setPosition({
            bgX + (bgWidth - textBounds.size.x) / 2.f - textBounds.position.x,
            bgY + (bgHeight - textBounds.size.y) / 2.f - textBounds.position.y - 2.f
        });

        window.draw(gameOverBg_);
        window.draw(gameOverText_);
    }
}
