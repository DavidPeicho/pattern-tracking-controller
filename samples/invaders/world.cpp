#include "world.hpp"

namespace ptc {

namespace invader {

size_t World::GAME_WIDTH = 800;
size_t World::GAME_HEIGHT = 640;
size_t World::PARTICLE_SIZE = 4;
float World::INIT_ARROW_SIDE_LEN = 4666.0f;
float World::ENEMY_SPEED = 16.0f;
float World::PLAYER_SPEED = 190.0f;
float World::BULLET_SPEED = 220.0f;
float World::TIME_BETWEEN_SHOT = 0.8f;
float World::TIME_BEFORE_RESET_ARROW = 0.45f;
float World::TIME_TEMPLATE_DETECTION = 2.0f;
float World::TIME_PLAYER_ANIM = 0.5f;
float World::TIME_SCALE_TITLE_ANIM = 1.5f;

float World::PARTICLE_SPEED = 68.0f;

size_t World::TEXT_SIZE_COUNTER = 64;
size_t World::TEXT_SIZE_GAMEOVER = 48;
size_t World::TEXT_SIZE_SCORE = 24;

size_t World::NB_ENEMIES = 55;

int World::TIME_BEFORE_START = 3;
int World::TIME_BEFORE_RESET = 5;

sf::Vector2f World::PLAYER_INIT = sf::Vector2f(0.0f, GAME_HEIGHT - 64.0f);

World::World(sf::RenderWindow& window, const sf::Font& font)
      : playerActor_{std::make_shared<MovingActor>(World::PLAYER_INIT,
                                                   sf::Vector2f(1.0, 1.0))}
      , window_{window}
      , particle_{sf::CircleShape(2)}
      , state_{World::State::MENU}
      , redColor_{sf::Color(231, 76, 60)}
      , lightGrayColor_{sf::Color(149, 165, 166)}
      , timerArrowInArea_{0.0f}
      , titleScaleUp_{false}
      , greenColor_{sf::Color(46, 204, 113)}
      , nbEnemies_{NB_ENEMIES}
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
  setupMenu();

  scoreText_.setFont(font);

  variableText_.setString("0");
  variableText_.setFont(font);

  gameoverUI_.setFont(font);
  gameoverUI_.setCharacterSize(TEXT_SIZE_GAMEOVER);

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

  ptc::Tracker::instance()->update();

  registerEvents();

}

void
World::update() {

  if (state_ == World::State::GAME) {
    updateGame();
  } else if (state_ == World::State::GAME_STARTING) {
    updateGameStarting();
  } else {
    updateMenu();
  }

}

void
World::draw() {

  if (state_ == World::State::GAME) {
    drawGame();
  } else if (state_ == World::State::GAME_STARTING) {
    drawGameStarting();
  } else {
    drawMenu();
  }

}

void
World::updateMenu() {

  deltaTime_ = float(clock_.restart().asMilliseconds()) / 1000.0f;

  updateParticles();

  if (!Tracker::instance()->update()) arrowSprite_.setScale(0.0f, 0.0f);

  // Updates title scale function of time to create
  // a dynamic menu.
  auto titleScale = titleSprite_.getScale();
  if (titleScaleUp_) {
    titleSprite_.setScale(titleScale.x + 0.06f * deltaTime_,
                          titleScale.y + 0.06f * deltaTime_);
  } else {
    titleSprite_.setScale(titleScale.x - 0.06f * deltaTime_,
                          titleScale.y - 0.06f * deltaTime_);
  }

  if (timerScaleTitle_.getElapsedTime().asSeconds() >= TIME_SCALE_TITLE_ANIM) {
    titleScaleUp_ = !titleScaleUp_;
    timerScaleTitle_.restart();
  }


  // Handles arrow template matching by
  // checking during a certain amount of time
  // the rotation and scale of the arrow.
  auto& arrowPoints = processor_->getArrowShape();
  if (arrowPoints.size() < 3) return;

  float dist = (arrowPoints[0].x - arrowPoints[1].x) *
               (arrowPoints[0].x - arrowPoints[1].x) +
               (arrowPoints[0].y - arrowPoints[1].y) *
               (arrowPoints[0].y - arrowPoints[1].y);


  float scale = dist / INIT_ARROW_SIDE_LEN;
  if (scale >= 3.0f) arrowSprite_.setScale(0.0f, 0.0f);

  float angle = processor_->getAngle() - 90.0f;
  // Checks whether arrow is in the template
  if (angle >= 32.0f && angle <= 50.0f && scale >= 0.85 && scale <= 1.2) {
    arrowSprite_.setColor(redColor_);
    timerArrowOutArea_.restart();
    timerArrowInArea_ += deltaTime_;
  }

  if (timerArrowOutArea_.getElapsedTime().asSeconds() >= TIME_BEFORE_RESET_ARROW) {
    arrowSprite_.setColor(lightGrayColor_);
    timerArrowInArea_ = 0.0;
  }

  // The players has kept the template
  // enough time in the template area.
  if (timerArrowInArea_ >= TIME_TEMPLATE_DETECTION) {
    setupGameStarting();
    changeState(state_);
    return;
  }

  // Changes arrowSprite rotation / scale
  arrowSprite_.setScale(scale, scale);
  arrowSprite_.setRotation(angle);

}

void
World::updateGameStarting() {

  deltaTime_ = float(clock_.restart().asMilliseconds()) / 1000.0f;

  updateParticles();

  int currTime = (int)timerBeforeStart_.getElapsedTime().asSeconds();
  int val = TIME_BEFORE_START - currTime;

  variableText_.setString(std::to_string(val));

  // Counter is over, the game starts
  if (currTime >= TIME_BEFORE_START) {
    setupGame();
    changeState(state_);
  }

}

void
World::updateGame() {

  auto& bulletsList = renderer_.getList("bullet");
  auto& enemiesList = renderer_.getList("entities");

  deltaTime_ = float(clock_.restart().asMilliseconds()) / 1000.0f;

  updateParticles();

  if (gameover_ &&
      timerBeforeStart_.getElapsedTime().asSeconds() >= TIME_BEFORE_RESET) {
    setupMenu();
    changeState(state_);
  }

  if (gameover_) return;

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
        variableText_.setString(std::to_string(score));

        --nbEnemies_;
        if (nbEnemies_ == 0) {
          setupWin();
          return;
        }

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

      bulletsList.erase(it++);

      setupGameover();
      break;
    }
  }

  playerActor_->setDelta(deltaTime_);
  // Resets player initial frame
  if (timerPlayerAnimation_.getElapsedTime().asSeconds() >= TIME_PLAYER_ANIM) {
    auto& renderable = renderer_.getList("player").front();
    renderable->setFrame(0);
  }

  ia_.update(deltaTime_, enemiesList, bulletsList, regionsPool_["bullet"]);

  Tracker::instance()->update();

  timeBeforeShot_ += deltaTime_;

}

void
World::updateParticles() {

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

}

void
World::drawMenu() {

  drawParticles();

  window_.draw(fixedArrowSprite_);
  window_.draw(arrowSprite_);
  window_.draw(titleSprite_);

}

void
World::drawGameStarting() {

  drawParticles();

  window_.draw(variableText_);

}

void
World::drawGame() {

  drawParticles();

  // Draws entities
  renderer_.render(window_);

  // Draws UI
  window_.draw(scoreText_);
  window_.draw(variableText_);

  if (gameover_) window_.draw(gameoverUI_);

}

void
World::drawParticles() {

  // Draws particles
  for (const auto& pos : particleList_) {
    particle_.setPosition(pos.x, pos.y);
    window_.draw(particle_);
  }

}

void
World::setupMenu() {

  auto& titleTex = *texturesPool_["title"];
  titleSprite_.setTexture(titleTex);
  titleSprite_.setOrigin(titleTex.getSize().x / 2, titleTex.getSize().y / 2);
  titleSprite_.setPosition(GAME_WIDTH / 2, titleTex.getSize().y / 2 + 20);
  titleSprite_.setScale(1.0f, 1.0f);

  auto& fixedArrowTex = *texturesPool_["arrow"];
  fixedArrowSprite_.setTexture(fixedArrowTex);
  fixedArrowSprite_.setOrigin(fixedArrowTex.getSize().x / 2,
                              fixedArrowTex.getSize().y / 2);
  fixedArrowSprite_.setScale(1.0f, 1.0f);
  fixedArrowSprite_.setRotation(45.0f);
  fixedArrowSprite_.setPosition((GAME_WIDTH / 2.0f), (GAME_HEIGHT / 2.0f));

  auto& arrowTex = *texturesPool_["arrow"];
  arrowSprite_.setTexture(arrowTex);
  arrowSprite_.setOrigin(arrowTex.getSize().x / 2, arrowTex.getSize().y / 2);
  arrowSprite_.setPosition((GAME_WIDTH / 2.0f), (GAME_HEIGHT / 2.0f));
  arrowSprite_.rotate(0.0f);
  arrowSprite_.setScale(0.0f, 0.0f);
  arrowSprite_.setColor(lightGrayColor_);

  timerArrowInArea_ = 0.0f;

}

void
World::setupGameStarting() {

  timerBeforeStart_.restart();

  size_t x = (GAME_WIDTH / 2) - TEXT_SIZE_COUNTER / 2;
  size_t y = (GAME_HEIGHT / 2) - TEXT_SIZE_COUNTER / 2;
  variableText_.setPosition(x, y);
  variableText_.setCharacterSize(TEXT_SIZE_COUNTER);

}

void
World::setupGameover() {

  size_t halfSize = TEXT_SIZE_GAMEOVER / 2;
  size_t halfWidth = (GAME_WIDTH / 2) - (8 * halfSize) / 2;

  gameover_ = true;
  gameoverUI_.setString("GAMEOVER");
  gameoverUI_.setColor(sf::Color::Red);
  gameoverUI_.setPosition(halfWidth, (GAME_HEIGHT - 42) / 2);

  timerBeforeStart_.restart();

}

void
World::setupWin() {

  size_t halfSize = TEXT_SIZE_GAMEOVER / 2;
  size_t halfWidth = (GAME_WIDTH / 2) - (8 * halfSize) / 2;

  gameoverUI_.setString("YOU WIN!");
  gameoverUI_.setColor(greenColor_);
  gameoverUI_.setPosition(halfWidth, (GAME_HEIGHT / 4) - halfSize);

  scoreText_.setCharacterSize(TEXT_SIZE_GAMEOVER);
  scoreText_.setString("SCORE: ");
  scoreText_.setPosition((GAME_WIDTH / 2) - (halfSize * 7) / 2,
                         (GAME_HEIGHT / 2) - halfSize);

  variableText_.setCharacterSize(TEXT_SIZE_GAMEOVER);
  variableText_.setPosition(halfWidth + 10 * halfSize,
                            (GAME_HEIGHT / 2) - halfSize);

  timerBeforeStart_.restart();

  gameover_ = true;

}

void
World::setupGame() {

  playerActor_->setPos(PLAYER_INIT);

  auto& bulletsList = renderer_.getList("bullet");
  auto& enemiesList = renderer_.getList("entities");
  for (auto& enemy : enemiesList) {
    enemy->setVisible(true);
  }

  bulletsList.clear();

  gameover_ = false;
  score = 0;
  nbEnemies_ = NB_ENEMIES;
  timeBeforeShot_ = 0.0f;
  deltaTime_ = 0.0f;

  scoreText_.setString("Score: ");
  scoreText_.setCharacterSize(TEXT_SIZE_SCORE);
  scoreText_.setPosition(4, 4);

  variableText_.setString("0");
  variableText_.setCharacterSize(TEXT_SIZE_SCORE);
  variableText_.setPosition(16, 32);

}

void
World::changeState(State s) {

  if (s == State::MENU) {
    state_ = State::GAME_STARTING;
  } else if (s == State::GAME_STARTING) {
    state_ = State::GAME;
  } else {
    state_ = State::MENU;
  }

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

  loadTexture("atlas", "samples/assets/invader-atlas.png");
  loadTexture("bullet", "samples/assets/bullet.png");
  loadTexture("arrow", "samples/assets/arrow.png");
  loadTexture("title", "samples/assets/title.png");

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
  auto& timerPlayerAnim = timerPlayerAnimation_;
  auto& renderer = renderer_;
  auto& pool = regionsPool_;

  processor_ = std::make_shared<ptc::event::InputProcessor>();
  processor_->registerEvent(ptc::event::Event::RIGHT, [&player]() -> void {

    player->moveRight();

  });
  processor_->registerEvent(ptc::event::Event::LEFT, [&player]() -> void {

    player->moveLeft();

  });
  processor_->registerEvent(ptc::event::Event::UP, [&timeShot, &player,
  &renderer, &pool, &timerPlayerAnim]() ->
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

    auto& playerRenderable = renderer.getList("player").front();
    playerRenderable->setFrame(1);

    timerPlayerAnim.restart();

  });

  auto tracker = ptc::Tracker::instance();
  tracker->inputProcessor(processor_);

}

void
World::loadTexture(const char* name, const char* path) {

  texturesPool_[name] = std::make_shared<sf::Texture>();
  auto &text = texturesPool_[name];
  if (!text->loadFromFile(path)) {
    std::string error = "File: impossible to load ";
    error += path;
    throw std::runtime_error(error);
  }
  text->setSmooth(true);

}

}

}
