#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

namespace ptc {

class VideoReader {

  public:
    void
    open();

    void
    close();

    cv::Mat
    nextFrame();

  public:
    int
    getWidth() const;

    int
    getHeight() const;

  private:
    cv::VideoCapture vCapture_;

};

}
