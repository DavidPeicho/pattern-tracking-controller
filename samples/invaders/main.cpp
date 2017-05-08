#include <string>
#include <unordered_map>

#include <SFML/Graphics.hpp>
#include <iostream>

#include <ptc.hpp>

#include "../game-engine/actor.hpp"
#include "../game-engine/texture-region.hpp"
#include "../game-engine/renderer.hpp"
#include "../game-engine/command.hpp"

#include "horizontal-actor.hpp"
#include "move-command.hpp"
#include "ia.hpp"

using namespace ptc::engine;
using namespace ptc::invader;
typedef std::shared_ptr<sf::Texture>                                TexturePtr;
typedef std::shared_ptr<TextureRegion>                              TextureRegionPtr;
typedef std::vector<std::shared_ptr<HorizontalActor>>               actorsList;

static size_t GAME_WIDTH  = 800;
static size_t GAME_HEIGHT = 640;
static size_t MAX_ROW_NB  = 5;

void
buildEnemyRow(actorsList& actorsList, Renderer& renderer,
              TextureRegionPtr& texture, size_t rowNbr);


int main() {

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
  atlas->setSmooth(true);

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
  actorsList enemies;
  buildEnemyRow(enemies, renderer, texturesPool["pizza"], 1);
  buildEnemyRow(enemies, renderer, texturesPool["cupcake"], 2);
  buildEnemyRow(enemies, renderer, texturesPool["cupcake"], 3);
  buildEnemyRow(enemies, renderer, texturesPool["donut"], 4);
  buildEnemyRow(enemies, renderer, texturesPool["donut"], 5);

  int enemiesDir = 1;

  IA ia(enemies);

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
    ia.update(delta.asSeconds());
    // Renders world
    window.clear(sf::Color(22, 29, 35));
    renderer.render(window);
    window.display();
  }

  //tracker->stop();

  return 0;

}

void
buildEnemyRow(actorsList& actorsList, Renderer& renderer,
              TextureRegionPtr& texture, size_t rowNbr) {

  for (size_t i = 1; i <= 11; ++i) {
    float startX = i * 48.0f + 56.0f + 32.0f;
    float startY = rowNbr * 42.0f + 32.0f;
    auto pos = sf::Vector2f(startX, startY);
    auto scale = sf::Vector2f(1.0, 1.0);
    actorsList.push_back(std::make_shared<HorizontalActor>(pos, scale));
    auto& actor = actorsList[actorsList.size() - 1];
    // Creates associate renderable, used to display the actor
    auto renderable = std::make_shared<Renderable>(*texture, *actor);
    renderer.enqueue(renderable);
  }

}
