#pragma once

#include <stdlib.h>
#include <memory>
#include <unordered_map>
#include <list>

#include <SFML/Graphics.hpp>
#include <input-processor.hpp>

#include <ptc.hpp>

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
    // Game constants
    static size_t MAX_NB_FRAMES_NOINPUT;
    static size_t GAME_WIDTH;
    static size_t GAME_HEIGHT;
    static size_t PARTICLE_SIZE;
    static float ENEMY_SPEED;
    static float PLAYER_SPEED;
    static float BULLET_SPEED;
    static float TIME_BETWEEN_SHOT;
    static float PARTICLE_SPEED;
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
    buildEnemiesColumn(size_t col);

    void
    createEnemy(TextureRegionPtr& texture, float x, float y);

    void
    registerTextures();

    void
    registerRegions();

    void
    registerEvents();

  private:
    ptc::engine::Renderer renderer_;

    // Assets pool
    std::unordered_map<std::string, TexturePtr>       texturesPool_;
    std::unordered_map<std::string, TextureRegionPtr> regionsPool_;

    // SFML Core
    sf::Clock clock_;
    sf::RenderWindow& window_;

    // UI
    sf::Text gameoverUI_;

    // Rendering
    sf::CircleShape particle_;

    // Inputs
    ptc::event::InputProcessor processor_;

    // Game variables
    MovingActorPtr playerActor_;
    IA ia_;

    std::list<sf::Vector2f> particleList_;

    float timeBeforeShot_ = 0.0f;
    float deltaTime_ = 0.0f;
    float lastRenderTime_ = 0.0f;
    bool  gameover_ = false;

    // Ui variables
    int score = 0;

};

} // namespace invader

} // namespace ptc
