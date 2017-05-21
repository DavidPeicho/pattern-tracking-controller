#pragma once 

#include <cstddef>
#include <memory>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace ptc {

namespace engine {

class TextureRegion {

  public:
    TextureRegion(sf::Texture& atlas, size_t nbFrames,
                  sf::IntRect rect);
  
  public:
    inline sf::Texture&
    getTexture() { return atlas_; }

    inline size_t
    getNbFrames() { return nbFrames_; }

    inline const sf::IntRect&
    getInitialRect() const { return rect_; }


  private:
    sf::Texture&  atlas_;
    sf::IntRect   rect_;
    size_t        nbFrames_;

};

} // namespace engine

} // namespace ptc
