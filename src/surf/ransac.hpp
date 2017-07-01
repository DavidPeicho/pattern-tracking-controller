//
// Created by leo on 5/21/17.
//

#pragma once

#include <opencv2/core/mat.hpp>

namespace ptc {
  namespace surf {
    class Ransac {
      public:
        static void findHomography(std::vector<cv::Point2f> &object, std::vector<cv::Point2f> &scene, cv::Mat &H);

      private:
        // Perform Gauss-Jordan elimination of coeffs matrix to compute H
        static void gaussJordan(cv::Mat_<double> &coeffs, cv::Mat_<double> &H);
        // Compute homography matrix H from in_points to out_points
        static void computeModel(std::vector<cv::Point2f> &in_points, std::vector<cv::Point2f> &out_points,
                                 cv::Mat_<double> &H);
        // Compute number of pairs of points from in_points to out_points that fit to the homography matrix H
        static int computeFittingPoints(std::vector<cv::Point2f> &in_points, std::vector<cv::Point2f> &out_points,
                                        cv::Mat_<double> &H, double decisionThreshold);
    };

  }
}
