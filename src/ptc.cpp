#include <ptc.hpp>
#include <ptc-impl.hpp>

namespace ptc {

Tracker::Tracker()
        : pimpl_{new TrackerImpl()} { }

Tracker::~Tracker() { }

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

void
Tracker::processFrame(cv::Mat &output, cv::Mat& input) {

  pimpl_->processFrame(output, input);

}

}
