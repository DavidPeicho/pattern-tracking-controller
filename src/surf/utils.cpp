//
// Created by leo on 5/3/17.
//

#include <opencv2/imgproc.hpp>
#include <iostream>
#include "utils.hpp"


namespace ptc {
  namespace surf {

    void Utils::drawFeaturePoints(cv::Mat &img, std::vector<ptc::surf::InterestPoint> featurePoints) {
      int x = 0, y = 0;
      std::cout << "img " << img.size().width << ":" << img.size().height << std::endl;
      std::cout << featurePoints.size() << std::endl;
      for (auto &e : featurePoints) {
        x = (int) (e.getX() / e.getScale());
        y = (int) (e.getY() / e.getScale());
        std::cout << x << ":" << y << ":" << e.getScale() << std::endl;
        cv::circle(img, cv::Point(x, y), (int) e.getScale() * 10, 0);
      }
    }

  }
}
