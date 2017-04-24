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
      Hessian(int nbOctaves, int intervalsPerOctave, int initSamplingStep, int hessianThreshold) :
          _nbOctaves(nbOctaves), _intervalsPerOctave(intervalsPerOctave), _initSamplinigStep(initSamplingStep),
          _hessianThreshold(hessianThreshold) {}
      void getInterestPoints(cv::Mat &img, std::vector<InterestPoint> &iPoints);


      private:
      bool isExtremum(int r, int c,
                             std::shared_ptr<ResponseLayer> b,
                             std::shared_ptr<ResponseLayer> m,
                             std::shared_ptr<ResponseLayer> t);

      void interpolateExtremum(int r, int c,
                                      std::shared_ptr<ResponseLayer> b,
                                      std::shared_ptr<ResponseLayer> m,
                                      std::shared_ptr<ResponseLayer> t,
                                      std::vector<InterestPoint> &featurePoints);

      void interpolateStep(int r, int c, std::shared_ptr<ResponseLayer> t, std::shared_ptr<ResponseLayer> m,
                                  std::shared_ptr<ResponseLayer> b, double *xi, double *xr, double *xc);

      CvMat *deriv3D(int r, int c, std::shared_ptr<ResponseLayer> t, std::shared_ptr<ResponseLayer> m,
                            std::shared_ptr<ResponseLayer> b);

      CvMat *hessian3D(int r, int c, std::shared_ptr<ResponseLayer> t, std::shared_ptr<ResponseLayer> m,
                              std::shared_ptr<ResponseLayer> b);


      private:
      int _nbOctaves;
      int _intervalsPerOctave;
      int _initSamplinigStep;
      int _hessianThreshold;

    };

  }

}

