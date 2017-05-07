#pragma once 

#include <cstddef>
#include <vector>
#include <memory>

#include <SFML/System/Vector2.hpp>

namespace ptc {

namespace engine {

class Renderable {

  

};

class Renderer {

  public:
    static int BASE_WIDTH;
    static int BASE_HEIGHT;

  public:
    Renderer();

    Renderer(size_t w, size_t h);

  public:
    void
    render();

    void
    resize(size_t newWidth, size_t newHeight);

  public:
    void
    enqueue(const std::shared_ptr<Renderable>& component);
    
  private:
    size_t                                    width_;
    size_t                                    height_;

    std::vector<std::shared_ptr<Renderable>>  components_;

};

} // namespace engine

} // namespace ptc
