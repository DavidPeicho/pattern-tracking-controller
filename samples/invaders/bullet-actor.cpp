#include "bullet-actor.hpp"

namespace ptc {

namespace invader {

  BulletActor::BulletActor(sf::Vector2f pos, sf::Vector2f dir)
              : MovingActor(pos, sf::Vector2f(1.0f, 1.0f))
              , dir_{dir} { }

  void
  BulletActor::update(float delta) {

    position_ += dir_ * moveSpeed_ * delta;

  }

} // namespace engine

} // namespace ptc
