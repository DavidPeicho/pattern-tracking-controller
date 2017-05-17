#include "move-command.hpp"

namespace ptc {

namespace invader {

void
MoveRightCommand::execute(std::shared_ptr<ptc::engine::Actor> actor) {

  auto horizontalActor = std::dynamic_pointer_cast<MovingActor>(actor);
  horizontalActor->moveRight();

}

void
MoveLeftCommand::execute(std::shared_ptr<ptc::engine::Actor> actor) {

  auto horizontalActor = std::dynamic_pointer_cast<MovingActor>(actor);
  horizontalActor->moveLeft();

}

} // namespace invader

} // namespace ptc
