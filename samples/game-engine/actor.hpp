#pragma once 

#include <SFML/System/Vector2.hpp>

namespace ptc {

namespace engine {

class Actor {

  public:
    Actor(sf::Vector2i pos, sf::Vector2i scale);

  public:
    virtual void update(float delta) = 0;

  protected:
    sf::Vector2i  scale_;
    sf::Vector2i  position_;
    float         angleDegree_;

};

} // namespace engine

} // namespace ptc
