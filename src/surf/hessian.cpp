//
// Created by leo on 4/19/17.
//

#include <processing/processing.hpp>
#include "hessian.hpp"
#include "response-map.hpp"


namespace ptc {
  namespace surf {

    std::vector<cv::Vec2i> Hessian::hessian(cv::Mat &img, int nbOctaves, int intervalsPerOctave, int initSamplingStep) {
      cv::Mat copy(img.size(), img.type());
      processing::integralImage(copy, img);
      std::vector<cv::Vec2i> featurePoints;
      getInterestPoints(img, featurePoints, nbOctaves, intervalsPerOctave, initSamplingStep);
      return featurePoints;
    }

    void Hessian::getInterestPoints(cv::Mat &img, std::vector<cv::Vec2i> &iPoints,
                                    int nbOctaves, int intervalsPerOctave, int initSamplingStep)
    {
      response rm(img, nbOctaves, intervalsPerOctave);
      rm.printResponseInfo();
      for (int i = 0; i < nbOctaves; i++) {
        for (int j = 0; j + 2 < intervalsPerOctave; j++) {

        }
      }
    }

  }
}

