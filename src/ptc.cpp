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

void
Tracker::preprocessFrame(const cv::Mat& input) {

  pimpl_->preprocessFrame(input);

}

bool
Tracker::processFrame(const cv::Mat& input) {

  return pimpl_->processFrame(input, inputProcessor_);

}

void
Tracker::inputProcessor(event::InputProcessor i) {

  this->inputProcessor_ = i;

}

const cv::Mat&
Tracker::getRawFrame() const {

  return pimpl_->getRawFrame();

}

const cv::Mat&
Tracker::getFrame(data::Frame frameType) const {

  return pimpl_->getFrame(frameType);

}

const std::vector<cv::Point>&
Tracker::arrowShape() const {

  return pimpl_->arrowShape();

}

}
