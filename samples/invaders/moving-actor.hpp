#pragma once 

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

#include "../game-engine/actor.hpp"

namespace ptc {

namespace invader {

class MovingActor : public ptc::engine::Actor {

  public:
    MovingActor(sf::Vector2f pos, sf::Vector2f scale);

  public:
    virtual void
    update(float delta);

  public:
    void
    moveLeft();

    void
    moveRight();

    void
    moveUp();

    void
    moveDown();

    void
    updateBBox();

  public:
    void
    setMoveSpeed(float s);

    void
    setDelta(float d);

    inline const sf::FloatRect&
    getBBox() const { return boundingBox_; }

    inline void
    setBBoxDim(float width, float height) {

      boundingBox_.width = width;
      boundingBox_.height = height;

    }

  protected:
    sf::FloatRect boundingBox_;

    float         moveSpeed_;
    float         delta_;

};

} // namespace engine

} // namespace ptc
