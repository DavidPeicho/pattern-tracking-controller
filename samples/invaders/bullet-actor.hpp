#pragma once 

#include <SFML/System/Vector2.hpp>

#include "moving-actor.hpp"

namespace ptc {

namespace invader {

class BulletActor : public ptc::invader::MovingActor {

  public:
    BulletActor(sf::Vector2f pos, sf::Vector2f dir);

  public:
    void
    update(float delta) override;

    inline bool
    isCreatedByPlayer() { return createdByPlayer_; }

    inline void
    setCreatedByPlayer(bool value) { createdByPlayer_ = value; }

  private:
    sf::Vector2f dir_;
    bool createdByPlayer_ = false;

};

} // namespace engine

} // namespace ptc
