//
// Created by leo on 5/21/17.
//

#pragma once

#include <opencv2/core/types.hpp>
#include <opencv2/core/mat.hpp>

namespace ptc {
  namespace surf {

    class Geometry {
      public:
      Geometry() = delete;

      public:
      static void perspectiveTransform(std::vector<cv::Point2f> &objCorners,
                                       std::vector<cv::Point2f> &sceneCorners, cv::Mat &H);
    };

  }
}


