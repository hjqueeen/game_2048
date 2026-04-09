#include <algorithm>
#include <filesystem>
#include <iostream>
#include <optional>
#include <vector>

#include "game2048.hpp"
#include "game_view.hpp"

int main() {
    // Create a fixed-size game window for desktop play.
    sf::RenderWindow window(
        sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}),
        "2048",
        sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    bool fontLoaded = false;
    const std::vector<std::filesystem::path> fontCandidates = {
        "assets/fonts/DejaVuSans-Bold.ttf",
        "../assets/fonts/DejaVuSans-Bold.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "/System/Library/Fonts/SFNSText.ttf",
        "/Library/Fonts/Arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"
    };

    // Try project-local font first, then OS fallback fonts.
    for (const auto& path : fontCandidates) {
        if (font.openFromFile(path.string())) {
            fontLoaded = true;
            break;
        }
    }

    if (!fontLoaded) {
        std::cerr << "No usable font found. Add one to assets/fonts/DejaVuSans-Bold.ttf\n";
        return 1;
    }

    Game2048 game;
    GameView view(font);
    bool gameOver = false;
    int bestScore = 0;
    sf::Clock frameClock;

    while (window.isOpen()) {
        // Delta time keeps animation speed stable across frame rates.
        float dt = frameClock.restart().asSeconds();
        game.updateAnimation(dt);

        // Poll and dispatch user/system events.
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else {
                view.processEvent(*event, game, gameOver);
            }
        }

        // Track session best score and render one complete frame.
        bestScore = std::max(bestScore, game.getScore());
        view.draw(window, game, gameOver, bestScore);
        window.display();
    }

    return 0;
}