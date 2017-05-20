#include <string>
#include <unordered_map>
#include <list>

#include <SFML/Graphics.hpp>
#include <iostream>

#include <ptc.hpp>

#include "world.hpp"

static size_t GAME_WIDTH = 800;
static size_t GAME_HEIGHT = 640;

int
main() {

  sf::RenderWindow window(sf::VideoMode(GAME_WIDTH, GAME_HEIGHT), "Invader",
                          sf::Style::Close);

  // Creates video tracker, and registers
  // the input processor used to move
  // the player around.
  auto tracker = ptc::Tracker::instance();
  tracker->start();

  sf::Font font;
  if (!font.loadFromFile("samples/assets/pixeldub.ttf")) {
    throw std::runtime_error("File: impossible to load pixeldub.ttf");
  }

  ptc::invader::World world(window, font);

  while (window.isOpen())
  {
    // World update
    world.update();
    // World rendering
    window.clear(sf::Color(22, 29, 35));
    world.draw();
    window.display();
  }

  tracker->stop();
  tracker->free();

  return 0;

}
