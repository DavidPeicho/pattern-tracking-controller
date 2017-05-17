#pragma once 

#include <SFML/System/Vector2.hpp>

#include "moving-actor.hpp"

namespace ptc {

namespace invader {

class BulletActor : public ptc::invader::MovingActor {

  public:
    BulletActor(sf::Vector2f pos, sf::Vector2f scale, sf::Vector2f dir);

  public:
    void
    update(float delta) override;

  private:
    sf::Vector2f dir_;

};

} // namespace engine

} // namespace ptc
