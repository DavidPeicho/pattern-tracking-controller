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
        // Compute homography matrix H from in_points to out_points and return the average error on the model
        static double computeModel(const std::vector<cv::Point2f> &in_points, const std::vector<cv::Point2f> &out_points,
                                 cv::Mat_<double> &Hest);
        // Compute number of pairs of points from in_points to out_points that fit to the homography matrix H
        static int computeFittingPoints(const std::vector<cv::Point2f> &in_points,
                                        const std::vector<cv::Point2f> &out_points,
                                        const cv::Mat_<double> &H, double decisionThreshold);
        static cv::Mat_<double> getHomographyLinearSystem(const std::vector<cv::Point2f> &in_points,
                                                          const std::vector<cv::Point2f> &out_points);

      public:
        static void printMat(const cv::Mat_<double> &mat);
    };

  }
}
