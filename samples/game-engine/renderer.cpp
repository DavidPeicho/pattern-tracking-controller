#include "renderer.hpp"

namespace ptc {

namespace engine {

int Renderer::BASE_WIDTH = 800;
int Renderer::BASE_HEIGHT = 640;

Renderable::Renderable(TextureRegion& textureRegion,
                       std::shared_ptr<Actor> actor)
           : textureRegion_{textureRegion}
           , actor_{actor} {

  sprite_.setTexture(textureRegion.getTexture());
  sprite_.setTextureRect(textureRegion.getInitialRect());

}

void
Renderable::render(sf::RenderWindow& window) {

  if (!visible_) return;

  sprite_.setPosition(actor_->getPos());
  sprite_.setScale(actor_->getScale());
  window.draw(sprite_);

}

std::shared_ptr<Actor>
Renderable::getActor() {

  return actor_;

}

void
Renderable::setFrame(int i) {

  int w = textureRegion_.getInitialRect().width;
  int h = textureRegion_.getInitialRect().height;

  sf::IntRect rect;
  rect.width = w;
  rect.height = h;
  rect.top = textureRegion_.getInitialRect().top;
  rect.left = textureRegion_.getInitialRect().left + i * w;

  sprite_.setTextureRect(rect);

}


Renderer::Renderer()
         : Renderer(BASE_WIDTH, BASE_HEIGHT) { }

Renderer::Renderer(size_t w, size_t h)
         : width_{w}
         , height_{h} { }

void
Renderer::render(sf::RenderWindow& window) {

  for (const auto& tuple : components_) {
    auto& list = tuple.second;
    for (const auto& component : list) {
      component->render(window);
    }
  }


}

void
Renderer::resize(size_t newWidth, size_t newHeight) {

  width_ = newWidth;
  height_ = newHeight;

}

void
Renderer::enqueue(std::string listId,
                  std::shared_ptr<Renderable>& component) {

  auto& list = components_[listId];
  list.push_front(component);

}

std::list<std::shared_ptr<Renderable>>&
Renderer::getList(std::string listId) {

  return components_[listId];

}


} // namespace engine

} // namespace ptc
