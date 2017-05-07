#include "horizontal-actor.hpp"

namespace ptc {

namespace engine {

HorizontalActor::HorizontalActor(sf::Vector2i pos, sf::Vector2i scale)
                : Actor(pos, scale) { }

void
HorizontalActor::update(float delta) { }

void
HorizontalActor::moveLeft(float delta) { }

void
HorizontalActor::moveRight(float delta) { }

} // namespace engine

} // namespace ptc
