#include <string>
#include <unordered_map>

#include <SFML/Graphics.hpp>
#include <iostream>

#include <ptc.hpp>

#include "../game-engine/actor.hpp"
#include "../game-engine/texture-region.hpp"
#include "horizontal-actor.hpp"

using namespace ptc::engine;
typedef std::shared_ptr<sf::Texture>    TexturePtr;
typedef std::shared_ptr<TextureRegion>  TextureRegionPtr;

int main() {

  sf::RenderWindow window(sf::VideoMode(800, 640), "Invader", sf::Style::Close);
  sf::Clock clock;

  // Loads the asset once, and creates TextureRegion for each actor.
  // This allows several actors to share the same TextureRegion, avoiding
  // several memory allocations.
  std::unordered_map<std::string, TexturePtr> textureAtlasPool;
  textureAtlasPool["atlas"] = std::make_shared<sf::Texture>();
  auto& atlas = textureAtlasPool["atlas"];
  if (!atlas->loadFromFile("samples/assets/invader-atlas.png")) {
    throw std::runtime_error("File: impossible to load invader-atlas.png");
  }

  // Creates every texture region from the atlas containing
  // every textures required by the game.
  std::unordered_map<std::string, TextureRegionPtr> texturesPool;
  texturesPool["pizza"] =
           std::make_shared<TextureRegion>(atlas, 3, sf::IntRect(0, 0, 32, 32));
  texturesPool["cupcake"] =
          std::make_shared<TextureRegion>(atlas, 2, sf::IntRect(0, 32, 32, 32));
  texturesPool["donut"] =
          std::make_shared<TextureRegion>(atlas, 2, sf::IntRect(0, 64, 32, 32));

  // Creates enemies
  std::vector<std::shared_ptr<ptc::engine::HorizontalActor>> enemies(55);
  for (size_t i = 0; i < 55; ++i) {
    auto pos = sf::Vector2i();
    auto scale = sf::Vector2i(1.0, 1.0);
    auto actor = std::make_shared<ptc::engine::HorizontalActor>(pos, scale);
  }

  while (window.isOpen())
  {
    sf::Time delta = clock.getElapsedTime();
    clock.restart();

    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    // Updates world
    window.clear(sf::Color(22, 29, 35));
    // Renders world
    window.display();
  }

  //tracker->stop();

  return 0;

}