#include "bullet-actor.hpp"

namespace ptc {

namespace invader {

  BulletActor::BulletActor(sf::Vector2f pos, sf::Vector2f scale, sf::Vector2f dir)
              : MovingActor(pos, scale)
              , dir_{dir} { }

  void
  BulletActor::update(float delta) {

    position_ += dir_ * 100.0f;

  }

} // namespace engine

} // namespace ptc
