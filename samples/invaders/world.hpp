#pragma once

#include <stdlib.h>
#include <memory>
#include <unordered_map>
#include <list>
#include <thread>

#include <SFML/Graphics.hpp>
#include <input-processor.hpp>

#include <ptc.hpp>

#include <chrono>

#include "../game-engine/actor.hpp"
#include "../game-engine/texture-region.hpp"
#include "../game-engine/renderer.hpp"
#include "../game-engine/command.hpp"

#include "moving-actor.hpp"
#include "bullet-actor.hpp"
#include "move-command.hpp"
#include "ia.hpp"

namespace ptc {

namespace invader {

class World {

  private:
    typedef std::shared_ptr<sf::Texture> TexturePtr;
    typedef std::shared_ptr<ptc::engine::TextureRegion> TextureRegionPtr;
    typedef std::unordered_map<std::string, TexturePtr> TexturePool;
    typedef std::unordered_map<std::string, TextureRegionPtr> TextureRegionPool;
    typedef std::shared_ptr<MovingActor>    MovingActorPtr;
    typedef std::vector<MovingActorPtr> actorsList;

  private:
    enum State {
      MENU,
      GAME,
      GAME_STARTING
    };

  private:
    // Game constants
    static size_t GAME_WIDTH;
    static size_t GAME_HEIGHT;
    static size_t PARTICLE_SIZE;

    static float INIT_ARROW_SIDE_LEN;
    static float ENEMY_SPEED;
    static float PLAYER_SPEED;
    static float BULLET_SPEED;

    static float TIME_BETWEEN_SHOT;
    static float TIME_BEFORE_RESET_ARROW;
    static float TIME_TEMPLATE_DETECTION;
    static float TIME_PLAYER_ANIM;
    static float TIME_SCALE_TITLE_ANIM;
    static float TIME_SHIELD_ACTIVATION;
    static float TIME_NEXT_SHIELD_ACTIVATION;

    static float PARTICLE_SPEED;

    static int TIME_BEFORE_START;
    static int TIME_BEFORE_RESET;

    static size_t TEXT_SIZE_COUNTER;
    static size_t TEXT_SIZE_SCORE;
    static size_t TEXT_SIZE_GAMEOVER;

    static size_t NB_ENEMIES;
    static size_t NB_SHIELDS;

    static sf::Vector2f PLAYER_INIT;

  public:
    World(sf::RenderWindow& window, const sf::Font& font);

  public:
    void
    update();

    void
    draw();

  private:
    void
    updateMenu();

    void
    updateGameStarting();

    void
    updateGame();

    void
    updateParticles();

    void
    drawMenu();

    void
    drawGameStarting();

    void
    drawGame();

    void
    drawParticles();

  private:
    void
    scheduleRecognition();

    void
    setupMenu();

    void
    setupGameStarting();

    void
    setupGameover();

    void
    setupWin();

    void
    setupGame();

    void
    changeState(State s);

    void
    buildEnemiesColumn(size_t col);

    void
    createEnemy(TextureRegionPtr& texture, float x, float y);

    void
    registerTextures();

    void
    registerRegions();

    void
    registerEvents();

    void
    loadTexture(const char* name, const char* path);

  private:
    ptc::engine::Renderer renderer_;

    // Assets pool
    std::unordered_map<std::string, TexturePtr>       texturesPool_;
    std::unordered_map<std::string, TextureRegionPtr> regionsPool_;

    // SFML Core
    sf::Clock clock_;
    sf::RenderWindow& window_;

    // UI Elements
    sf::Sprite  titleSprite_;
    sf::Sprite  fixedArrowSprite_;
    sf::Sprite  arrowSprite_;
    sf::Text    scoreText_;
    sf::Text    shieldsText_;
    sf::Text    variableText_;
    sf::Text    gameoverUI_;

    sf::Color   redColor_;
    sf::Color   lightGrayColor_;
    sf::Color   greenColor_;

    // Timers
    sf::Clock   timerScaleTitle_;
    sf::Clock   timerArrowOutArea_;
    sf::Clock   timerBeforeStart_;
    sf::Clock   timerPlayerAnimation_;
    sf::Clock   timerShieldActivation_;
    sf::Clock   timerNextShield_;

    float       timerArrowInArea_;
    bool        titleScaleUp_;

    // Rendering
    sf::CircleShape particle_;

    // Inputs
    std::shared_ptr<ptc::event::InputProcessor> processor_;

    // States
    State state_;

    // Game variables
    MovingActorPtr  playerActor_;
    MovingActorPtr  shieldActor_;
    IA              ia_;

    std::list<sf::Vector2f> particleList_;

    float timeBeforeShot_ = 0.0f;
    float deltaTime_ = 0.0f;
    bool  gameover_ = false;

    size_t nbEnemies_;
    size_t nbShields_;

    // Ui variables
    int score = 0;

    std::shared_ptr<std::thread> testT_;

};

} // namespace invader

} // namespace ptc
