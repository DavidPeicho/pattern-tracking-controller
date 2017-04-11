#include <ptc.hpp>
#include <ptc-impl.hpp>

namespace ptc {

Tracker::~Tracker() {

}

Tracker::Tracker()
{

  pimpl_ = std::make_shared<TrackerImpl>();

}

void
Tracker::start() {

  pimpl_->start();

}

void
Tracker::update() {

  pimpl_->update();

}

void
Tracker::stop() {

  pimpl_->stop();

}

std::shared_ptr<cv::Mat>
Tracker::processFrame(cv::Mat& input) {

  return pimpl_->processFrame(input);

}

}
