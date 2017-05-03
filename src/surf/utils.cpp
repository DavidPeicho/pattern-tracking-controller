//
// Created by leo on 5/3/17.
//

#include <opencv2/imgproc.hpp>
#include "utils.hpp"


namespace ptc {
  namespace surf {

    void Utils::drawFeaturePoints(cv::Mat &img, std::vector<ptc::surf::InterestPoint> featurePoints) {
      int x = 0, y = 0;
      for (auto &e : featurePoints) {
        x = e.getX();
        y = e.getY();
        cv::circle(img, cv::Point(x, y), (int) e.getScale() * 10, 0);
      }
    }

  }
}
