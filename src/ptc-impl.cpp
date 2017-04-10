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

  processing::convert(output, input);


}

}
