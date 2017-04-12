#pragma once

#include <opencv2/opencv.hpp>

namespace ptc {

namespace utils {

namespace maths {

template<typename T>
const T
clamp(const T &val, const T &min, const T &max) {

  return (val < min) ? min : ((val > max) ? max : val);

}

template<typename T>
bool
inBounds(T val, const std::tuple<T, T>& tup) {

  return val >= std::get<0>(tup) && val <= std::get<1>(tup);

}

inline bool
inBounds(int x, int y, int h, int w) {

  return (x >= 0 && y >= 0 && x < w && y < h);

}

inline bool
inRange(const cv::Point& a, const cv::Point& b, double& dist) {

  return ((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y)) <= dist;

}

inline double
distanceToLine(cv::Point lineBegin, cv::Point lineEnd, cv::Point p) {

  cv::Point l;
  l.x = lineEnd.y - lineBegin.y;
  l.y = lineEnd.x - lineBegin.x;

  return abs(l.x * p.x - l.y * p.y + lineEnd.x * lineBegin.y - lineEnd.y *
             lineBegin.x) / sqrt(l.x * l.x + l.y * l.y);

}

inline bool
checkArrowBase(std::vector<double> angles, size_t prev, size_t next,
               std::tuple<double, double> acuteBounds,
               std::tuple<double, double> rightBounds) {

  if (!inBounds(angles[prev], acuteBounds) ||
      !inBounds(angles[next], acuteBounds)) {
    return false;
  }

  size_t nextNext = (next != angles.size() - 1) ? next + 1 : 0;
  size_t prevPrev = (prev != 0) ? prev - 1 : angles.size() - 1;

  if (!inBounds(angles[prevPrev], rightBounds) ||
      !inBounds(angles[nextNext], rightBounds)) {
    return false;
  }
  return true;
}

inline double
getAngle(cv::Point origin, cv::Point p1, cv::Point p2) {

  cv::Point2d vec1(p1 - origin);
  cv::Point2d vec2(p2 - origin);

  double normVec1 = sqrt(vec1.x * vec1.x + vec1.y * vec1.y);
  double normVec2 = sqrt(vec2.x * vec2.x + vec2.y * vec2.y);

  vec1.x = vec1.x / normVec1;
  vec1.y = vec1.y / normVec1;
  vec2.x = vec2.x / normVec2;
  vec2.y = vec2.y / normVec2;

  double angle = vec1.x * vec2.x + vec1.y * vec2.y;
  if (angle >= 1.0) {
    angle = 0.0;
  } else if (angle <= -1.0) {
    angle = M_PI;
  } else {
    angle = acos(angle);
  }

  return abs((angle * 180.0) / M_PI);

}

} // namespace maths

} // namespace utils

} // namespace ptc