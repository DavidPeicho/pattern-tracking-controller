#include <ptc.hpp>
#include <ptc-impl.hpp>

namespace ptc {

Tracker* Tracker::instance_ = nullptr;

Tracker::~Tracker() {

}

Tracker::Tracker()
        : inputProcessor_{nullptr}
{

  pimpl_ = std::make_shared<TrackerImpl>();

}

void
Tracker::start() {

  pimpl_->start();

}

bool
Tracker::update() {

  if (inputProcessor_ == nullptr) return false;

  return pimpl_->update(inputProcessor_);

}

void
Tracker::stop() {

  pimpl_->stop();

}

void
Tracker::inputProcessor(std::shared_ptr<event::InputProcessor> i) {

  this->inputProcessor_ = i;

}

void
Tracker::setDebugFrame(std::shared_ptr<cv::Mat>& frame) {

  pimpl_->setDebugFrame(frame);

}

const cv::Mat&
Tracker::getRawFrame() const {

  return pimpl_->getRawFrame();

}

const cv::Mat&
Tracker::getFrame(data::Frame frameType) const {

  return pimpl_->getFrame(frameType);

}

int
Tracker::getWidth() const {

  return pimpl_->getWidth();

}

int
Tracker::getHeight() const {

  return pimpl_->getHeight();

}

const std::vector<cv::Point>&
Tracker::arrowShape() const {

  return pimpl_->arrowShape();

}

}
