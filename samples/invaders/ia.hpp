#pragma once 

#include <vector>
#include <memory>
#include <list>
#include <cstdlib>
#include <ctime>
#include <iostream>


#include "../game-engine/command.hpp"
#include "../game-engine/renderer.hpp"
#include "../game-engine/texture-region.hpp"

#include "moving-actor.hpp"
#include "bullet-actor.hpp"
#include "move-command.hpp"

namespace ptc {

namespace invader {

class IA {

  private:
    typedef std::shared_ptr<ptc::engine::Command>   CmdPtr;
    typedef std::shared_ptr<MoveLeftCommand>        MoveLeftCmdPtr;
    typedef std::shared_ptr<MoveRightCommand>       MoveRightCmdPtr;

  private:
    enum Dir {
      RIGHT,
      LEFT
    };

  public:
    IA();

  public:
    void
    update(float delta,
           const std::list<std::shared_ptr<ptc::engine::Renderable>>&,
           std::list<std::shared_ptr<ptc::engine::Renderable>>&,
           const std::shared_ptr<ptc::engine::TextureRegion>&);

  private:
    void
    shot(const std::list<std::shared_ptr<ptc::engine::Renderable>>&,
         std::list<std::shared_ptr<ptc::engine::Renderable>>&,
         const std::shared_ptr<ptc::engine::TextureRegion>&);

  private:
    static unsigned int MAX_NB_MOVES;

  private:
    MoveLeftCmdPtr  leftCmd_;
    MoveRightCmdPtr rightCmd_;

    Dir             dir_;

    unsigned int    nbMoves_;

    float           elapsed_;
    float           elapsedShot_;

    bool            playedAnim_;

};

} // namespace invader

} // namespace ptc
