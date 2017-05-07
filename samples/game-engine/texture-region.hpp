#pragma once 

#include <cstddef>
#include <memory>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace ptc {

namespace engine {

class TextureRegion {

  public:
    TextureRegion(std::shared_ptr<sf::Texture>& atlas, size_t nbFrames,
                  sf::IntRect rect);
  
  private:
    std::shared_ptr<sf::Texture>&   atlas_;
    sf::IntRect                     rect_;
    size_t                          nbFrames_;

};

} // namespace engine

} // namespace ptc
