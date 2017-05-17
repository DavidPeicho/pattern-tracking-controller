#include <string>
#include <unordered_map>
#include <list>

#include <SFML/Graphics.hpp>
#include <iostream>

#include <ptc.hpp>

#include "../game-engine/actor.hpp"
#include "../game-engine/texture-region.hpp"
#include "../game-engine/renderer.hpp"
#include "../game-engine/command.hpp"

#include "moving-actor.hpp"
#include "bullet-actor.hpp"
#include "move-command.hpp"
#include "ia.hpp"

using namespace ptc::engine;
using namespace ptc::invader;
typedef std::shared_ptr<sf::Texture>                                TexturePtr;
typedef std::shared_ptr<TextureRegion>                              TextureRegionPtr;
typedef std::unordered_map<std::string, TextureRegionPtr>           TexturePool;
typedef std::vector<std::shared_ptr<MovingActor>>                   actorsList;

// Game constants
static size_t GAME_WIDTH  = 800;
static size_t GAME_HEIGHT = 640;
static size_t MAX_ROW_NB  = 5;
static float  ENEMY_SPEED = 1000.0f;
static float  PLAYER_SPEED = 250.0f;

void
buildEnemyRow(actorsList& actorsList, Renderer& renderer,
              TextureRegionPtr& texture, size_t rowNbr);

void
registerTextures(TexturePool& pool, TexturePtr& atlas);

void
registerEvents(ptc::event::InputProcessor& inputProcessor,
               std::shared_ptr<MovingActor>& player);

int
main() {

  sf::Clock clock;
  sf::RenderWindow window(sf::VideoMode(GAME_WIDTH, GAME_HEIGHT), "Invader",
                          sf::Style::Close);

  // Creates video tracker, and registers
  // the input processor used to move
  // the player arround.
  auto tracker = ptc::Tracker::instance();
  tracker->start();

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
  registerTextures(texturesPool, atlas);

  // Creates bullets list
  std::list<std::shared_ptr<BulletActor>> bulletsList;

  // Creates enemies
  actorsList enemies;
  buildEnemyRow(enemies, renderer, texturesPool["pizza"], 1);
  buildEnemyRow(enemies, renderer, texturesPool["cupcake"], 2);
  buildEnemyRow(enemies, renderer, texturesPool["cupcake"], 3);
  buildEnemyRow(enemies, renderer, texturesPool["donut"], 4);
  buildEnemyRow(enemies, renderer, texturesPool["donut"], 5);
  IA ia(enemies, bulletsList);

  // Creates Player
  auto playerActor = 
           std::make_shared<MovingActor>(sf::Vector2f(0.0, GAME_HEIGHT - 64.0f),
                                         sf::Vector2f(1.0, 1.0));
  playerActor->setMoveSpeed(PLAYER_SPEED);
  auto playerRenderer = std::make_shared<Renderable>(*texturesPool["tomatoe"],
                                                     *playerActor);
  renderer.enqueue(playerRenderer);

  // Registers inputs moving the player arround.
  ptc::event::InputProcessor processor;
  registerEvents(processor, playerActor);
  tracker->inputProcessor(processor);

  float deltaTime = 0.0;
  while (window.isOpen())
  {
    sf::Time delta = clock.getElapsedTime();
    clock.restart();
    deltaTime = delta.asSeconds();

    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    ///////////////////
    // Updates world //
    ///////////////////
    ia.update(deltaTime);
    // Updates bullet list
    for (auto& bullet : bulletsList) {
      bullet->update(deltaTime);
    }

    playerActor->setDelta(deltaTime);
    tracker->update();

    // Renders world
    window.clear(sf::Color(22, 29, 35));
    renderer.render(window);
    window.display();
  }

  tracker->stop();
  tracker->free();

  return 0;

}

void
registerTextures(TexturePool& pool, TexturePtr& atlas) {

  pool["pizza"] =
          std::make_shared<TextureRegion>(*atlas, 3, sf::IntRect(0, 0, 32, 32));
  pool["cupcake"] =
         std::make_shared<TextureRegion>(*atlas, 2, sf::IntRect(0, 32, 32, 32));
  pool["donut"] =
         std::make_shared<TextureRegion>(*atlas, 2, sf::IntRect(0, 64, 32, 32));
  pool["tomatoe"] =
         std::make_shared<TextureRegion>(*atlas, 2, sf::IntRect(0, 96, 32, 32));

}

void
registerEvents(ptc::event::InputProcessor& processor,
               std::shared_ptr<MovingActor>& player) {

  /*processor.registerEvent(ptc::event::Event::UP,
                          [playerActor, bulletsList]() -> void {

    std::shared_ptr<BulletActor> bullet =
                            std::make_shared<BulletActor>(playerActor->getPos(),
                                                        playerActor->getScale(),
                                                      sf::Vector2f(0.0f, 1.0f));
    bulletsList.push_front(bullet);

  });*/

  processor.registerEvent(ptc::event::Event::RIGHT,
                          [player]() -> void {

                            std::cout << "RIGHT" << std::endl;
    player->moveRight();

  });
  processor.registerEvent(ptc::event::Event::LEFT,
                          [player]() -> void {

                            std::cout << "LEFT" << std::endl;
    player->moveLeft();

  });

}

void
buildEnemyRow(actorsList& actorsList, Renderer& renderer,
              TextureRegionPtr& texture, size_t rowNbr) {

  for (size_t i = 1; i <= 11; ++i) {
    float startX = i * 48.0f + 56.0f + 32.0f;
    float startY = rowNbr * 42.0f + 32.0f;
    auto pos = sf::Vector2f(startX, startY);
    auto scale = sf::Vector2f(1.0, 1.0);
    actorsList.push_back(std::make_shared<MovingActor>(pos, scale));
    auto& actor = actorsList[actorsList.size() - 1];
    actor->setMoveSpeed(ENEMY_SPEED);
    // Creates associate renderable, used to display the actor
    auto renderable = std::make_shared<Renderable>(*texture, *actor);
    renderer.enqueue(renderable);
  }

}
