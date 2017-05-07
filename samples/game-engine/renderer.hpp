#pragma once 

#include <cstddef>
#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>

#include "actor.hpp"
#include "texture-region.hpp"

namespace ptc {

namespace engine {

class Renderable {

  public:
    Renderable(TextureRegion& textureRegion, const Actor& actor);

  public:
    void
    render(sf::RenderWindow& window);

  private:
    const Actor&          actor_;
    TextureRegion&        textureRegion_;
    sf::Sprite            sprite_;

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
    render(sf::RenderWindow& window);

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
