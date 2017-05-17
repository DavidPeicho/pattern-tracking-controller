#include "moving-actor.hpp"

namespace ptc {

namespace invader {

MovingActor::MovingActor(sf::Vector2f pos, sf::Vector2f scale)
                : ptc::engine::Actor(pos, scale) { }

void
MovingActor::update(float delta) { }

void
MovingActor::moveLeft() {

  position_.x -= delta_ * moveSpeed_;

}

void
MovingActor::moveUp() {

  position_.y -= delta_ * moveSpeed_;

}

void
MovingActor::moveDown() {

  position_.y += delta_ * moveSpeed_;

}

void
MovingActor::moveRight() {

  position_.x += delta_ * moveSpeed_;

}

void
MovingActor::setDelta(float d) {

  delta_ = d;

}

void
MovingActor::setMoveSpeed(float s) {

  moveSpeed_ = s;

}

} // namespace engine

} // namespace ptc
