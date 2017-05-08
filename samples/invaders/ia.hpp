#pragma once 

#include <vector>
#include <memory>

#include "../game-engine/command.hpp"

#include "horizontal-actor.hpp"
#include "move-command.hpp"

namespace ptc {

namespace invader {

class IA {

  private:
    typedef std::shared_ptr<ptc::engine::Command> CmdPtr;
    typedef std::shared_ptr<MoveLeftCommand>      MoveLeftCmdPtr;
    typedef std::shared_ptr<MoveRightCommand>      MoveRightCmdPtr;

  private:
    enum Dir {
      RIGHT,
      LEFT
    };

  public:
    IA(const std::vector<std::shared_ptr<HorizontalActor>>& actors);

  public:
    void
    update(float delta);

  private:
    static unsigned int MAX_NB_MOVES;

  private:
    const std::vector<std::shared_ptr<HorizontalActor>>&  actors_;
    
    MoveLeftCmdPtr                                        leftCmd_;
    MoveRightCmdPtr                                       rightCmd_;
    
    Dir                                                   dir_;

    unsigned int                                          nbMoves_;

    float                                                 elapsed_;

};

} // namespace invader

} // namespace ptc
