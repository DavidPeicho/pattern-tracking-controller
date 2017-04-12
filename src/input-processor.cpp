#include <input-processor.hpp>

namespace ptc {

namespace event {

InputProcessor::InputProcessor() { }

void
InputProcessor::registerEvent(Event e, std::function<void()> callback) {

  events_[e].push_back(callback);

}

void
InputProcessor::unregisterEvent(Event e) {

  if (events_[e].size()) events_[e].pop_back();

}

void
InputProcessor::call(Event e) {

  auto& vec = events_[e];
  for (const auto& callback : vec) {
    callback();
  }

}

bool
InputProcessor::isRegistered(Event e) {

  auto& vec = events_[e];
  return vec.size() != 0;

}

void
InputProcessor::setArrowShape(const std::vector<cv::Point>& points) {

  arrowShape_ = points;

}

} // namespace event

} // namespace ptc
