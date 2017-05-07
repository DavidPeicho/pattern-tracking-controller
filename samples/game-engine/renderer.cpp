#include "renderer.hpp"

namespace ptc {

namespace engine {

  int Renderer::BASE_WIDTH = 800;
  int Renderer::BASE_HEIGHT = 640;

  Renderer::Renderer()
           : Renderer(BASE_WIDTH, BASE_HEIGHT) { }

  Renderer::Renderer(size_t w, size_t h)
           : width_{w}
           , height_{h} { }

  void
  Renderer::render() { }

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
