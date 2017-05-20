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
    bool
    isRegistered(Event e);

    inline void
    setTemplate(std::vector<cv::Point> points) { template_ = points; }

    inline const std::vector<cv::Point>&
    getTemplate() { return template_; }

    void
    setArrowShape(const std::vector<cv::Point>& points);

    inline const std::vector<cv::Point>&
    getArrowShape() const { return arrowShape_; }

    inline void
    setAngle(double a) { angle_ = a; }

    inline double
    getAngle() { return angle_; }

    inline void
    setMinMatchDistance(float minDist) { minMatchDist_ = minDist; }

    inline float
    getMinMatchDistance() { return minMatchDist_; }

  private:
    std::unordered_map<uint, std::vector<std::function<void()>>>  events_;

    std::vector<cv::Point>                                        arrowShape_;
    std::vector<cv::Point>                                        template_;

    double                                                        angle_;
    float                                                         minMatchDist_;

};

} // namespace event

} // namespace ptc
