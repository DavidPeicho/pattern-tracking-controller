#pragma once

#include <input/video-reader.hpp>

namespace ptc {

class TrackerImpl {

  public:
    void
    start();

    void
    update();

    void
    stop();

  private:
    VideoReader v_;

};

}
