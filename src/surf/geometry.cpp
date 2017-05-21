//
// Created by leo on 5/21/17.
//

#include <iostream>
#include "geometry.hpp"

namespace ptc {
  namespace surf {

    void Geometry::perspectiveTransform(std::vector<cv::Point2f> &objCorners, std::vector<cv::Point2f> &sceneCorners,
                                        cv::Mat &H) {
      for (cv::Point2f &point : objCorners) {
        double w = H.at<double>(2, 0) * point.x + H.at<double>(2, 1) * point.y + H.at<double>(2, 2) * 1;
        sceneCorners.push_back(cv::Point2f(
             (float)((H.at<double>(0, 0) * point.x + H.at<double>(0, 1) * point.y + H.at<double>(0, 2) * 1) / w),
             (float)((H.at<double>(1, 0) * point.x + H.at<double>(1, 1) * point.y + H.at<double>(1, 2) * 1) / w)));
      }
    }
  }
}
