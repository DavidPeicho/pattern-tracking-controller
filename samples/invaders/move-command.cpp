#include "move-command.hpp"

namespace ptc {

namespace invader {

void
MoveRightCommand::execute(std::shared_ptr<ptc::engine::Actor> actor,
                          float delta) {

  auto horizontalActor = std::dynamic_pointer_cast<HorizontalActor>(actor);
  horizontalActor->moveRight(delta);

}

void
MoveLeftCommand::execute(std::shared_ptr<ptc::engine::Actor> actor,
                          float delta) {

  auto horizontalActor = std::dynamic_pointer_cast<HorizontalActor>(actor);
  horizontalActor->moveLeft(delta);

}

} // namespace invader

} // namespace ptc
