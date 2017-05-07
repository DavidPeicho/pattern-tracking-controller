#include "renderer.hpp"

namespace ptc {

namespace engine {

  int Renderer::BASE_WIDTH = 800;
  int Renderer::BASE_HEIGHT = 640;

  Renderable::Renderable(TextureRegion& textureRegion,
                         const Actor& actor)
             : textureRegion_{textureRegion}
             , actor_{actor} {

    sprite_.setTexture(textureRegion.getTexture());
    sprite_.setTextureRect(textureRegion.getInitialRect());

  }

  void
  Renderable::render(sf::RenderWindow& window) {

    sprite_.setPosition(actor_.getPos());
    sprite_.setScale(actor_.getScale());
    window.draw(sprite_);

  }

  Renderer::Renderer()
           : Renderer(BASE_WIDTH, BASE_HEIGHT) { }

  Renderer::Renderer(size_t w, size_t h)
           : width_{w}
           , height_{h} { }

  void
  Renderer::render(sf::RenderWindow& window) {

    for (const auto& component : components_) component->render(window);

  }

  void
  Renderer::resize(size_t newWidth, size_t newHeight) {

    width_ = newWidth;
    height_ = newHeight;

  }

  void
  Renderer::enqueue(const std::shared_ptr<Renderable>& component) {

    components_.push_back(component);

  }

} // namespace engine

} // namespace ptc
