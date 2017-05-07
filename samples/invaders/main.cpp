#include <string>
#include <unordered_map>

#include <SFML/Graphics.hpp>
#include <iostream>

#include <ptc.hpp>

#include "../game-engine/actor.hpp"
#include "../game-engine/texture-region.hpp"
#include "../game-engine/renderer.hpp"
#include "horizontal-actor.hpp"

using namespace ptc::engine;
typedef std::shared_ptr<sf::Texture>    TexturePtr;
typedef std::shared_ptr<TextureRegion>  TextureRegionPtr;

int main() {

  static size_t GAME_WIDTH = 800;
  static size_t GAME_HEIGHT = 640;

  sf::RenderWindow window(sf::VideoMode(GAME_WIDTH, GAME_HEIGHT),
                          "Invader",
                          sf::Style::Close);
  sf::Clock clock;

  // Creates world renderer in charge of drawing every actor
  Renderer renderer;

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
          std::make_shared<TextureRegion>(*atlas, 3, sf::IntRect(0, 0, 32, 32));
  texturesPool["cupcake"] =
         std::make_shared<TextureRegion>(*atlas, 2, sf::IntRect(0, 32, 32, 32));
  texturesPool["donut"] =
         std::make_shared<TextureRegion>(*atlas, 2, sf::IntRect(0, 64, 32, 32));

  std::vector<std::shared_ptr<Actor>> actors;

  // Creates enemies
  std::vector<std::shared_ptr<ptc::engine::HorizontalActor>> enemies(55);
  for (size_t i = 0; i < 55; ++i) {
    // Creates associated actor, dealing with game data
    float startX = (float)(GAME_WIDTH / (5 - (i / 11))) - 64.0f;
    float startY = GAME_HEIGHT / (5 - (i / 11)) - 32.0f;
    auto pos = sf::Vector2f(startX + i * 64.0f, startY);
    auto scale = sf::Vector2f(1.0, 1.0);
    actors.push_back(std::make_shared<HorizontalActor>(pos, scale));
    auto& actor = actors[actors.size() - 1];
    // Creates associate renderable, used to display the actor
    auto txtPtr = texturesPool["pizza"];
    if (i >= 11 && i < 33) {
      txtPtr = texturesPool["cupcake"];
    } else if (i >= 33) {
      txtPtr = texturesPool["donut"];
    }
    auto renderable = std::make_shared<Renderable>(*txtPtr, *actor);
    renderer.enqueue(renderable);

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
    renderer.render(window);
    window.display();
  }

  //tracker->stop();

  return 0;

}