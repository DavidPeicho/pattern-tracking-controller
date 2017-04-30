//
// Created by leo on 4/19/17.
//

#pragma once 

#include <vector>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include <memory>
#include "response-layer.hpp"

namespace ptc {

  namespace surf {

    class Hessian {
      public:
      Hessian() = delete;
      static std::vector<cv::Vec2i> hessian(cv::Mat &img, int nbOctaves, int intervalsPerOctave, int initSamplingStep);

      private:
      static void getInterestPoints(cv::Mat &img, std::vector<cv::Vec2i> &iPoints,
                                    int nbOctaves, int intervalsPerOctave, int initSamplingStep);
      static bool isExtremum(int r, int c,
                             std::shared_ptr<ResponseLayer> b,
                             std::shared_ptr<ResponseLayer> m,
                             std::shared_ptr<ResponseLayer> t);
      static void interpolateExtremum(int r, int c,
                                      std::shared_ptr<ResponseLayer> b,
                                      std::shared_ptr<ResponseLayer> m,
                                      std::shared_ptr<ResponseLayer> t);
    };

  }

}

