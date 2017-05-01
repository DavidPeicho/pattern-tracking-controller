//
// Created by leo on 4/19/17.
//

#pragma once 

#include <vector>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include <memory>
#include "response-layer.hpp"
#include "interest-point.hpp"

namespace ptc {

  namespace surf {

    class Hessian {
      public:
      Hessian() = delete;
      static void hessian(cv::Mat &img, int nbOctaves, int intervalsPerOctave, int initSamplingStep,
                          std::vector<interest> &featurePoints);

      private:
      static void getInterestPoints(cv::Mat &img, std::vector<interest> &iPoints,
                                    int nbOctaves, int intervalsPerOctave, int initSamplingStep);
      static bool isExtremum(int r, int c,
                             std::shared_ptr<ResponseLayer> b,
                             std::shared_ptr<ResponseLayer> m,
                             std::shared_ptr<ResponseLayer> t);
      static void interpolateExtremum(int r, int c,
                                      std::shared_ptr<ResponseLayer> b,
                                      std::shared_ptr<ResponseLayer> m,
                                      std::shared_ptr<ResponseLayer> t,
                                      std::vector<interest> &featurePoints);

      static void interpolateStep(int r, int c, std::shared_ptr<ResponseLayer> t, std::shared_ptr<ResponseLayer> m,
                                  std::shared_ptr<ResponseLayer> b, double *xi, double *xr, double *xc);

      static CvMat *deriv3D(int r, int c, std::shared_ptr<ResponseLayer> t, std::shared_ptr<ResponseLayer> m,
                            std::shared_ptr<ResponseLayer> b);

      static CvMat *hessian3D(int r, int c, std::shared_ptr<ResponseLayer> t, std::shared_ptr<ResponseLayer> m,
                              std::shared_ptr<ResponseLayer> b);
    };

  }

}

