#include <input/video-reader.hpp>

namespace ptc {

void
VideoReader::open() {

  int i = 0;
  for (; i < 7; ++i) {
    if (vCapture_.open(i)) break;
  }

  if (i == 7) throw std::logic_error("PTC: No camera has been detected.");

}

cv::Mat
VideoReader::nextFrame() {

  if (!vCapture_.isOpened()) {
    throw std::logic_error("PTC: VideoReader is not started.");
  }

  cv::Mat frame;
  vCapture_ >> frame;

  return frame;
}

void
VideoReader::close() {

  vCapture_.release();

}

int
VideoReader::getWidth() const {

  return (int)vCapture_.get(CV_CAP_PROP_FRAME_WIDTH);

}

int
VideoReader::getHeight() const {

  return (int)vCapture_.get(CV_CAP_PROP_FRAME_HEIGHT);

}

}
