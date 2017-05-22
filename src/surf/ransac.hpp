//
// Created by leo on 5/21/17.
//

#pragma once

#include <opencv2/core/mat.hpp>

namespace ptc {
  namespace surf {
    class Ransac {
      static void findHomography(std::vector<cv::Point2f> &object, std::vector<cv::Point2f> &scene, cv::Mat &H);
      static void gaussJordan(cv::Mat_<double> &coeffs, cv::Mat_<float> &H);
    };

  }
}
