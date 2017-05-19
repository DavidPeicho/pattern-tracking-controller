#include "world.hpp"

namespace ptc {

namespace invader {

size_t World::MAX_NB_FRAMES_NOINPUT = 10;
size_t World::GAME_WIDTH = 800;
size_t World::GAME_HEIGHT = 640;
size_t World::PARTICLE_SIZE = 4;
float World::ENEMY_SPEED = 16.0f;
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
  gameoverUI_.setColor(sf::Color::Red);
  gameoverUI_.setPosition((GAME_WIDTH / 2) - 132, (GAME_HEIGHT - 42) / 2);

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

  deltaTime_ = float(clock_.restart().asMilliseconds()) / 1000.0f;

  sf::Event event;
  while (window_.pollEvent(event))
  {
    if (event.type == sf::Event::Closed)
      window_.close();
  }

  // Updates bullet list
  std::list<std::shared_ptr<ptc::engine::Renderable>>::const_iterator it;
  for (it = bulletsList.begin(); it != bulletsList.end(); ++it) {
    auto bullet = std::dynamic_pointer_cast<BulletActor>((*it)->getActor());
    bool collided = false;

    // Enemy has been shot
    for (auto& enemy : enemiesList) {

      if (!enemy->isVisible()) continue;

      auto a = std::dynamic_pointer_cast<MovingActor>(enemy->getActor());
      if (bullet->isCreatedByPlayer() &&
          a->getBBox().intersects(bullet->getBBox())) {
        enemy->setVisible(false);
        collided = true;

        // Updates score
        score += 50;
        //scoreValueText.setString(std::to_string(score));
        break;
      }
    }
    if (collided) {
      bulletsList.erase(it++);
      continue;
    }

    if (bullet->getPos().y < 0 || bullet->getPos().y > GAME_HEIGHT) {
      bulletsList.erase(it++);
      continue;
    }
    bullet->update(deltaTime_);

    // Player has been shot
    if (!bullet->isCreatedByPlayer() &&
        playerActor_->getBBox().intersects(bullet->getBBox())) {
      gameover_ = true;
      bulletsList.erase(it++);
      break;
    }
  }

  playerActor_->setDelta(deltaTime_);
  ia_.update(deltaTime_, enemiesList, bulletsList, regionsPool_["bullet"]);
  //Tracker::instance()->update();

  // DEBUG
  playerActor_->setPos(sf::Mouse::getPosition().x, playerActor_->getPos().y);
  playerActor_->setMoveSpeed(1.0f);
  playerActor_->moveRight();
  if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
  {
    processor_.call(ptc::event::UP);
  }
  // END DEBUG

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

  // DEBUG
  /*auto& bulletsList = renderer_.getList("bullet");
  std::list<std::shared_ptr<ptc::engine::Renderable>>::const_iterator it;
  uint8_t t = 0;
  for (it = bulletsList.begin(); it != bulletsList.end(); ) {
    auto p = std::dynamic_pointer_cast<MovingActor>((*it)->getActor());
    sf::RectangleShape s;
    s.setSize(sf::Vector2f(p->getBBox().width, p->getBBox().height));
    s.setPosition(p->getBBox().left, p->getBBox().top);
    s.setFillColor(sf::Color(t, t, t, 255));
    window_.draw(s);
    std::advance(it, 5);
    t += 24;
  }*/

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

  processor_.registerEvent(ptc::event::Event::RIGHT, [&player]() -> void {

    player->moveRight();

  });
  processor_.registerEvent(ptc::event::Event::LEFT, [&player]() -> void {

    player->moveLeft();

  });
  processor_.registerEvent(ptc::event::Event::UP, [&timeShot, &player,
  &renderer, &pool]() ->
    void {

    if (timeShot < World::TIME_BETWEEN_SHOT) return;

    auto pos = sf::Vector2f(player->getPos().x + 8,
                            player->getPos().y - 16);
    auto bullet = std::make_shared<BulletActor>(pos, sf::Vector2f(0.0f, -1.0f));
    bullet->setMoveSpeed(World::BULLET_SPEED);
    bullet->setBBoxDim(12.0f, 12.0f);
    bullet->setCreatedByPlayer(true);

    auto renderable = std::make_shared<ptc::engine::Renderable>
      (*pool["bullet"], bullet);

    renderer.enqueue("bullet", renderable);
    timeShot = 0.0f;

  });
  auto tracker = ptc::Tracker::instance();
  tracker->inputProcessor(processor_);

}

}

}
