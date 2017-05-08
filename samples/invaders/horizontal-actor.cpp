#include "horizontal-actor.hpp"

namespace ptc {

namespace invader {

HorizontalActor::HorizontalActor(sf::Vector2f pos, sf::Vector2f scale)
                : ptc::engine::Actor(pos, scale) { }

void
HorizontalActor::update(float delta) { }

void
HorizontalActor::moveLeft(float delta) {

  position_.x -= delta * 1000.0f;

}

void
HorizontalActor::moveRight(float delta) {

  position_.x += delta * 1000.0f;

}

} // namespace engine

} // namespace ptc
