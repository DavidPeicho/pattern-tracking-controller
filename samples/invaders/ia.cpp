#include "ia.hpp"

namespace ptc {

namespace invader {

unsigned int IA::MAX_NB_MOVES = 8;

IA::IA()
   : leftCmd_{std::make_shared<MoveLeftCommand>()}
   , rightCmd_{std::make_shared<MoveRightCommand>()}
   , dir_{IA::Dir::RIGHT}
   , nbMoves_{IA::MAX_NB_MOVES - 1}
   , elapsed_{0.0f}
   , elapsedShot_{0.0f} { }

void
IA::update(float delta,
           const std::list<std::shared_ptr<ptc::engine::Renderable>>& actors,
           std::list<std::shared_ptr<ptc::engine::Renderable>>& bullets,
           const std::shared_ptr<ptc::engine::TextureRegion>& bulletTexture) {

  elapsed_ += delta;
  elapsedShot_ += delta;

  if (delta <= 0.005f) return;

  static float TIME_BEFORE_MV = 1.0f;
  static float TIME_BEFORE_SHOT = 3.0f;

  if (elapsed_ >= TIME_BEFORE_MV) {
    IA::CmdPtr cmd = nullptr;
    if (dir_ == IA::Dir::RIGHT) {
      cmd = rightCmd_;
    } else if (dir_ == IA::Dir::LEFT) {
      cmd = leftCmd_;
    }
    // Updates every enemies
    for (auto& actor : actors) {
      if (!actor->isVisible()) continue;
      auto a = std::dynamic_pointer_cast<MovingActor>(actor->getActor());
      a->setDelta(1.0f);
      cmd->execute(a);
    }
    elapsed_ = 0.0f;

    // Changes direction whevener the enemies have
    // reached the other side.
    --nbMoves_;
    if (nbMoves_ == 0) {
      if (dir_ == IA::Dir::RIGHT) {
        dir_ = IA::Dir::LEFT;
      } else {
        dir_ = IA::Dir::RIGHT;
      }
      nbMoves_ = MAX_NB_MOVES;
    }
  }

  if (elapsedShot_ >= TIME_BEFORE_SHOT) {
    shot(actors, bullets, bulletTexture);
    elapsedShot_ = 0.0f;
  }

}

void
IA::shot(const std::list<std::shared_ptr<ptc::engine::Renderable>>& actors,
         std::list<std::shared_ptr<ptc::engine::Renderable>>& bullets,
         const std::shared_ptr<ptc::engine::TextureRegion>& bulletTexture) {

  int random = std::rand() % 11;
  int startIdx = random * 5;

  auto it = std::next(actors.begin(), startIdx);
  auto end = std::next(actors.begin(), startIdx + 5);
  for ( ; it != end ; ++it) {
    if (!(*it)->isVisible()) continue;

    auto actor = (*it)->getActor();
    auto pos = sf::Vector2f(actor->getPos().x + 16, actor->getPos().y + 32);
    auto bullet = std::make_shared<BulletActor>(pos, sf::Vector2f(0.0f, 1.0f));
    bullet->setMoveSpeed(220.0f);
    bullet->setBBoxDim(12.0f, 12.0f);
    bullet->setCreatedByPlayer(false);
    auto renderable = std::make_shared<ptc::engine::Renderable>(*bulletTexture,
                                                   bullet);
    bullets.push_front(renderable);
    break;
  }

}

} // namespace invader

} // namespace ptc
