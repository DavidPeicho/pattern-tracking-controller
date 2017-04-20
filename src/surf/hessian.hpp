//
// Created by leo on 4/19/17.
//

#pragma once 

#include <vector>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include <memory>

namespace ptc {

  namespace surf {

    class Hessian {
      public:
      Hessian() = delete;
      static std::vector<cv::Vec2i> hessian(cv::Mat &img, int nbOctaves, int intervalsPerOctave, int initSamplingStep);

      private:
      static void getInterestPoints(cv::Mat &img, std::vector<cv::Vec2i> &iPoints,
                                    int nbOctaves, int intervalsPerOctave, int initSamplingStep);
      static void getResponseLayer(cv::Mat &img, int step, int filterSize);
      static double getBoxIntegral(cv::Mat &img, int row, int col, int rows, int cols);

    };

  }

}

