#pragma once

#include <memory>

#include <input/video-reader.hpp>
#include <processing/processing.hpp>
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

    std::shared_ptr<cv::Mat>
    processFrame(cv::Mat& input);

  private:
    static int MIN_SIZE;

  private:

    VideoReader v_;


};

}
