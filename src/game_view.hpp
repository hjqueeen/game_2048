#pragma once

#include <SFML/Graphics.hpp>

#include <optional>
#include <vector>

#include "game2048.hpp"

constexpr int TILE_SIZE = 100;
constexpr int PADDING = 10;
constexpr int WINDOW_WIDTH = GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * PADDING + 20;
constexpr int WINDOW_HEIGHT = GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * PADDING + 100;

class GameView {
public:
    explicit GameView(const sf::Font& font);

    bool processEvent(const sf::Event& event, Game2048& game, bool& gameOver);
    void draw(sf::RenderWindow& window, const Game2048& game, bool gameOver, int bestScore);

private:
    const sf::Font& font_;

    sf::RectangleShape bgShape_;
    sf::RectangleShape gridBg_;
    std::vector<sf::RectangleShape> cellBackgrounds_;
    std::vector<sf::RectangleShape> tileShapes_;
    std::vector<sf::Text> tileTexts_;

    sf::Text scoreLabel_;
    sf::Text scoreValue_;
    sf::Text bestLabel_;
    sf::Text bestValue_;
    sf::Text gameOverText_;
    sf::RectangleShape gameOverBg_;

    sf::RectangleShape newGameButton_;
    sf::Text newGameText_;
};
