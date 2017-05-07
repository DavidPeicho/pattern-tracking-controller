#include "texture-region.hpp"

namespace ptc {

namespace engine {

  TextureRegion::TextureRegion(std::shared_ptr<sf::Texture>& atlas,
                               size_t nbFrames, sf::IntRect rect)
                : atlas_{atlas}
                , nbFrames_{nbFrames_}
                , rect_{rect} { }

} // namespace engine

} // namespace ptc
