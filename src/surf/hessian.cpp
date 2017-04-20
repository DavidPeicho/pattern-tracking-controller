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
        getResponseLayer(*layer, step, filterSize);
        filterSize += filterSizeIncrease;
        layers.push_back(layer);
      }
      for (int i = 1; i < nbOctaves; ++i) {
        for (int j = 0; j < 2; j++) {
          auto layer = new cv::Mat(w, h, img.type());
          getResponseLayer(*layer, step, filterSize);
          layers.push_back(layer);
          filterSize += filterSizeIncrease;
        }
        step *= 2;
        w /= 2;
        h /= 2;
        filterSizeIncrease *= 2;
      }
    }

    double Hessian::getBoxIntegral(cv::Mat &img, int row, int col, int rows, int cols) {
      // The subtraction by one for row/col is because row/col is inclusive.
      float A = img.at<uchar>(row, col);
      float B = img.at<uchar>(row, col + cols);
      float C = img.at<uchar>(row + rows, col);
      float D = img.at<uchar>(row + rows, cols);
      return std::max(0.f, A - B - C + D);
    }

    void Hessian::getResponseLayer(cv::Mat &img, int step, int filterSize) {
      int lobe = filterSize / 3;
      int band = filterSize / 9 * 5;
      double Dxx, Dxy, Dyy;
      double nFactor = 1 / (filterSize * filterSize);
      for (int j = 0; j < img.size().height; j++)
        for (int i = 0; i < img.size().width; i++ ) {
          int r = j * step;
          int c = i * step;
          Dxx = getBoxIntegral(img, r - lobe + 1, c - band, 2 * lobe - 1, filterSize)
                - getBoxIntegral(img, r - lobe + 1, c - lobe / 2, 2 * lobe - 1, lobe) * 3;
          Dyy = getBoxIntegral(img, r - band, c - lobe + 1, filterSize, 2 * lobe - 1)
                - getBoxIntegral(img, r - lobe / 2, c - lobe + 1, lobe, 2 * lobe - 1) * 3;
          Dxy = + getBoxIntegral(img, r - lobe, c + 1, lobe, lobe)
                + getBoxIntegral(img, r + 1, c - lobe, lobe, lobe)
                - getBoxIntegral(img, r - lobe, c - lobe, lobe, lobe)
                - getBoxIntegral(img, r + 1, c + 1, lobe, lobe);
          Dxx *= nFactor;
          Dyy *= nFactor;
          Dxy *= nFactor;
          img.at<uchar>(r, c) = Dxx * Dyy - 0.81 * Dxy * Dxy;
        }
    }


  }
}

