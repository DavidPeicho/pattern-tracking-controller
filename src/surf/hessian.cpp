//
// Created by leo on 4/19/17.
//

#include <processing/processing.hpp>
#include "hessian.hpp"


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
      int step = initSamplingStep;
      int filterSize = 9;
      int w = img.cols;
      int h = img.rows;
      int filterSizeIncrease = 6;
      std::vector<cv::Mat*> layers;
      for (int j = 0; j < 2; j++) {
        auto layer = new cv::Mat(w, h, img.type());
        filterSize += filterSizeIncrease;
        layers.push_back(layer);
      }
      for (int i = 1; i < nbOctaves; ++i) {
        for (int j = 0; j < 2; j++) {
          auto layer = new cv::Mat(w, h, img.type());
          filterSize += filterSizeIncrease;
          layers.push_back(layer);
        }
        step *= 2;
        w /= 2;
        h /= 2;
        filterSizeIncrease *= 2;
      }
    }

  }
}

