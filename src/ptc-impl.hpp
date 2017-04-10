#pragma once

#include <input/video-reader.hpp>
#include <processing/convolution.hpp>

namespace ptc {

class TrackerImpl {

  public:
    void
    start();

    void
    update();

    void
    stop();

    void
    processFrame(cv::Mat& output, cv::Mat& input);

  public:
    VideoReader v_;

};

}
