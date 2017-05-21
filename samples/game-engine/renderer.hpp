#pragma once 

#include <cstddef>
#include <unordered_map>
#include <list>
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
    Renderable(TextureRegion& textureRegion,
               std::shared_ptr<Actor> actor);

  public:
    void
    render(sf::RenderWindow& window);

  public:
    std::shared_ptr<Actor>
    getActor();

    void
    setFrame(int i);

    inline void
    setVisible(bool v) { visible_ = v; }

    inline bool
    isVisible() { return visible_; }

  private:
    std::shared_ptr<Actor>  actor_;
    const TextureRegion&    textureRegion_;
    sf::Sprite              sprite_;

    bool                    visible_ = true;
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
    enqueue(std::string listId, std::shared_ptr<Renderable>& component);

    std::list<std::shared_ptr<Renderable>>&
    getList(std::string listId);
    
  private:
    size_t                                    width_;
    size_t                                    height_;

    std::unordered_map<std::string,
      std::list<std::shared_ptr<Renderable>>> components_;

};

} // namespace engine

} // namespace ptc
