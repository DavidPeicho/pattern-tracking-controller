#include "moving-actor.hpp"

namespace ptc {

namespace invader {

MovingActor::MovingActor(sf::Vector2f pos, sf::Vector2f scale)
                : ptc::engine::Actor(pos, scale) {

  boundingBox_.width = 32.0f;
  boundingBox_.height = 32.0f;
  updateBBox();

}

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
  updateBBox();

}

void
MovingActor::moveRight() {

  position_.x += delta_ * moveSpeed_;
  updateBBox();

}

void
MovingActor::setDelta(float d) {

  delta_ = d;

}

void
MovingActor::setMoveSpeed(float s) {

  moveSpeed_ = s;

}

void
MovingActor::updateBBox() {

  boundingBox_.left = position_.x;
  boundingBox_.top = position_.y;

}

} // namespace engine

} // namespace ptc
