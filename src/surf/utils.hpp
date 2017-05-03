//
// Created by leo on 5/3/17.
//

#pragma once 

#include <vector>
#include <opencv2/core/mat.hpp>
#include "interest-point.hpp"

namespace ptc {
  namespace surf {

    class Utils {
      public:
      Utils() = delete;
      public:
      static void drawFeaturePoints(cv::Mat &img, std::vector<InterestPoint> featurePoints);

    };

  }
}
