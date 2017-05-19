#include "world.hpp"

namespace ptc {

namespace invader {

size_t World::MAX_NB_FRAMES_NOINPUT = 10;
size_t World::GAME_WIDTH = 800;
size_t World::GAME_HEIGHT = 640;
size_t World::PARTICLE_SIZE = 4;
float World::ENEMY_SPEED = 500.0f;
float World::PLAYER_SPEED = 190.0f;
float World::BULLET_SPEED = 220.0f;
float World::TIME_BETWEEN_SHOT = 0.7f;
float World::PARTICLE_SPEED = 50.0f;
sf::Vector2f World::PLAYER_INIT = sf::Vector2f(0.0f, GAME_HEIGHT - 64.0f);

World::World(sf::RenderWindow& window, const sf::Font& font)
      : playerActor_{std::make_shared<MovingActor>(World::PLAYER_INIT,
                                                   sf::Vector2f(1.0, 1.0))}
      , window_{window}
      , particle_{sf::CircleShape(2)}
{

  // Loads the asset once, and creates TextureRegion for each actor.
  // This allows several actors to share the same TextureRegion, avoiding
  // several memory allocations.
  std::unordered_map<std::string, TexturePtr> textureAtlasPool;
  registerTextures();

  // Creates every texture region from the atlas containing
  // every textures required by the game.
  std::unordered_map<std::string, TextureRegionPtr> texturesPool;
  registerRegions();

  // Creates UI elements
  gameoverUI_.setFont(font);
  gameoverUI_.setString("GAMEOVER");
  gameoverUI_.setCharacterSize(48);
  gameoverUI_.setPosition(GAME_WIDTH / 2 - 192, GAME_HEIGHT / 2);

  registerEvents();

  // Creates enemies
  for (size_t i = 0; i < 11; ++i) buildEnemiesColumn(i);

  // Creates player
  playerActor_->setMoveSpeed(World::PLAYER_SPEED);
  auto playerRenderable = std::make_shared<ptc::engine::Renderable>
    (*regionsPool_["tomatoe"], playerActor_);
  renderer_.enqueue("player", playerRenderable);

  // Init particle system
  size_t nbParticles = GAME_HEIGHT / (World::PARTICLE_SIZE + 8);
  for (size_t i = 0; i < nbParticles; ++i) {
    int x = std::rand() % World::GAME_WIDTH;
    int y = i * (World::PARTICLE_SIZE + 8);
    particleList_.push_front(sf::Vector2f(x, y));
  }

}

void
World::update() {

  if (gameover_) return;

  auto& bulletsList = renderer_.getList("bullet");
  auto& enemiesList = renderer_.getList("entities");

  //deltaTime_ = clock_.getElapsedTime().asSeconds();
  deltaTime_ = clock_.restart().asSeconds();

  //std::cout << deltaTime_ << std::endl;

  sf::Event event;
  while (window_.pollEvent(event))
  {
    if (event.type == sf::Event::Closed)
      window_.close();
  }

  ia_.update(deltaTime_, enemiesList, bulletsList, regionsPool_["bullet"]);

  // Updates bullet list
  std::list<std::shared_ptr<ptc::engine::Renderable>>::const_iterator it;
  for (it = bulletsList.begin(); it != bulletsList.end(); ++it) {
    auto bullet = std::dynamic_pointer_cast<BulletActor>((*it)->getActor());
    bool collided = false;

    // Enemy has been shot
    for (auto& enemy : enemiesList) {
      if (!enemy->isVisible()) continue;

      auto a = std::dynamic_pointer_cast<MovingActor>(enemy->getActor());
      if (a->getBBox().contains(bullet->getPos())) {
        enemy->setVisible(false);
        bulletsList.erase(it++);
        collided = true;

        // Updates score
        score += 50;
        //scoreValueText.setString(std::to_string(score));
      }
    }
    if (collided) continue;

    // Player has been shot
    if (playerActor_->getBBox().contains(bullet->getPos())) {
      gameover_ = true;
      bulletsList.erase(it++);
      break;
    }

    if (bullet->getPos().y < 0 || bullet->getPos().y > GAME_HEIGHT) {
      bulletsList.erase(it++);
    }
    bullet->update(deltaTime_);
  }

  playerActor_->setDelta(deltaTime_);
  Tracker::instance()->update();

  // Updates particle list
  std::list<sf::Vector2f>::iterator partIt;
  for (partIt = particleList_.begin(); partIt != particleList_.end(); ++partIt) {
    auto& pos = (*partIt);
    pos += sf::Vector2f(0.0f, World::PARTICLE_SPEED * deltaTime_);
  }
  // Creates new particle whenever one ides
  if (particleList_.front().y >= World::GAME_HEIGHT) {
    particleList_.erase(particleList_.begin());
    particleList_.push_back(sf::Vector2f(std::rand() % World::GAME_WIDTH, 0));
  }

  timeBeforeShot_ += deltaTime_;

}

void
World::draw() {

  // Draws particles
  for (const auto& pos : particleList_) {
    particle_.setPosition(pos.x, pos.y);
    window_.draw(particle_);
  }
  // Draws entities
  renderer_.render(window_);
  if (gameover_) window_.draw(gameoverUI_);

}

void
World::buildEnemiesColumn(size_t col) {

  createEnemy(regionsPool_["pizza"],
              col * 48.0f + 56.0f + 32.0f, 1 * 42.0f + 32.0f);
  createEnemy(regionsPool_["cupcake"],
              col * 48.0f + 56.0f + 32.0f, 2 * 42.0f + 32.0f);
  createEnemy(regionsPool_["cupcake"],
              col * 48.0f + 56.0f + 32.0f, 3 * 42.0f + 32.0f);
  createEnemy(regionsPool_["donut"],
              col * 48.0f + 56.0f + 32.0f, 4 * 42.0f + 32.0f);
  createEnemy(regionsPool_["donut"],
              col * 48.0f + 56.0f + 32.0f, 5 * 42.0f + 32.0f);

}

void
World::createEnemy(World::TextureRegionPtr& texture, float x, float y) {

  auto pos = sf::Vector2f(x, y);
  auto scale = sf::Vector2f(1.0, 1.0);

  auto actor = std::make_shared<MovingActor>(pos, scale);
  actor->setMoveSpeed(ENEMY_SPEED);

  auto renderable = std::make_shared<ptc::engine::Renderable>(*texture, actor);
  renderer_.enqueue("entities", renderable);

}

void
World::registerTextures() {

  texturesPool_["atlas"] = std::make_shared<sf::Texture>();
  texturesPool_["bullet"] = std::make_shared<sf::Texture>();

  auto &atlas = texturesPool_["atlas"];
  if (!atlas->loadFromFile("samples/assets/invader-atlas.png")) {
    throw std::runtime_error("File: impossible to load invader-atlas.png");
  }
  auto &bullet = texturesPool_["bullet"];
  if (!bullet->loadFromFile("samples/assets/bullet.png")) {
    throw std::runtime_error("File: impossible to load bullet.png");
  }

  atlas->setSmooth(true);
  bullet->setSmooth(true);

}

void
World::registerRegions() {

  auto& atlas = texturesPool_["atlas"];
  auto& bullet = texturesPool_["bullet"];

  // Registers enemies TextureRegion
  regionsPool_["pizza"] =
    std::make_shared<ptc::engine::TextureRegion>(*atlas, 3,
                                                 sf::IntRect(0, 0, 32,32));
  regionsPool_["cupcake"] =
    std::make_shared<ptc::engine::TextureRegion>(*atlas, 2,
                                                 sf::IntRect(0, 32, 32, 32));
  regionsPool_["donut"] =
    std::make_shared<ptc::engine::TextureRegion>(*atlas, 2,
                                                 sf::IntRect(0, 64, 32, 32));
  // Registers player TextureRegion
  regionsPool_["tomatoe"] =
    std::make_shared<ptc::engine::TextureRegion>(*atlas, 2,
                                                 sf::IntRect(0, 96, 32, 32));
  // Registers bullet
  regionsPool_["bullet"] =
    std::make_shared<ptc::engine::TextureRegion>(*bullet, 1,
                                                 sf::IntRect(0, 0, 12, 12));

}

void
World::registerEvents() {

  auto& player = playerActor_;
  auto& timeShot = timeBeforeShot_;
  auto& renderer = renderer_;
  auto& pool = regionsPool_;

  processor.registerEvent(ptc::event::Event::RIGHT, [&player]() -> void {

    player->moveRight();

  });
  processor.registerEvent(ptc::event::Event::LEFT, [&player]() -> void {

    player->moveLeft();

  });
  processor.registerEvent(ptc::event::Event::UP, [&timeShot, &player,
  &renderer, &pool]() ->
    void {

    if (timeShot < World::TIME_BETWEEN_SHOT) return;

    auto pos = sf::Vector2f(player->getPos().x + 8,
                            player->getPos().y - 16);
    auto bullet = std::make_shared<BulletActor>(pos, sf::Vector2f(0.0f, -1.0f));
    bullet->setMoveSpeed(World::BULLET_SPEED);

    auto renderable = std::make_shared<ptc::engine::Renderable>
      (*pool["bullet"], bullet);

    renderer.enqueue("bullet", renderable);
    timeShot = 0.0f;

  });
  auto tracker = ptc::Tracker::instance();
  tracker->inputProcessor(processor);

}

}

}
