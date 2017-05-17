#pragma once 

#include <SFML/System/Vector2.hpp>

namespace ptc {

namespace engine {

class Actor {

  public:
    Actor(sf::Vector2f pos, sf::Vector2f scale);

  public:
    virtual void
    update(float delta) = 0;

  public:
    inline const sf::Vector2f&
    getPos() const { return position_; }

    inline const sf::Vector2f&
    getScale() const { return scale_; }

  protected:
    sf::Vector2f  position_;
    sf::Vector2f  scale_;
    float         angleDegree_;

};

} // namespace engine

} // namespace ptc
