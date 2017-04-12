#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include <opencv2/core/types.hpp>

namespace ptc {

namespace event {

enum Event {

  NONE = 0,
  UP,
  DOWN,
  LEFT,
  RIGHT,
  AT

};

class InputProcessor {

  public:
    InputProcessor();

  void
    call(Event e);

  public:
    void
    registerEvent(Event e, std::function<void()>);

    void
    unregisterEvent(Event e);

  public:
    void
    setArrowShape(const std::vector<cv::Point>& points);

  public:
    bool
    isRegistered(Event e);

    inline std::vector<cv::Point>&
    getArrowShape() {

      return arrowShape_;

    }

  private:
    std::unordered_map<uint, std::vector<std::function<void()>>>  events_;

    std::vector<cv::Point>                                        arrowShape_;

};

} // namespace event

} // namespace ptc
