#pragma once 

#include <memory>

#include "../game-engine/command.hpp"
#include "../game-engine/actor.hpp"

#include "horizontal-actor.hpp"

namespace ptc {

namespace invader {

class MoveRightCommand : public ptc::engine::Command {

  public:
    void
    execute(std::shared_ptr<ptc::engine::Actor> actor, float delta) override;

};

class MoveLeftCommand : public ptc::engine::Command {

  public:
    void
    execute(std::shared_ptr<ptc::engine::Actor> actor, float delta) override;

};

} // namespace invader

} // namespace ptc
