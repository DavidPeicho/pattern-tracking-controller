#include <ptc-impl.hpp>

namespace ptc {

void
TrackerImpl::start() {

  v_.open();

}

void
TrackerImpl::update() {

  auto frame = v_.nextFrame();

}

void
TrackerImpl::stop() {

  v_.close();

}

void
TrackerImpl::processFrame(cv::Mat& output, cv::Mat& input) {

  cv::Mat gray = cv::Mat::zeros(input.size().height,
                                input.size().width,
                                CV_8UC1);
  processing::grayscale(gray, input);

  cv::Mat grayDownscaled = cv::Mat::zeros(input.size().height / 2,
                                          input.size().width / 2,
                                          CV_8UC1);
  //processing::downscaleBy(grayDownscaled, gray, 2.0);
  processing::downscaleBy(output, gray, 2.0);


  //processing::binarize(output, grayDownscaled);

}

}
