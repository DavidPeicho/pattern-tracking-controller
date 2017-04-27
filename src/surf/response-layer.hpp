//
// Created by leo on 4/24/17.
//

#pragma once 

#include <opencv2/core/mat.hpp>
#include <memory>

namespace ptc {
  namespace surf {
    class ResponseLayer {
      public:
      ResponseLayer(int filterSize, int width, int height, int step) : _filterSize(filterSize), _step(step) {
        data = std::make_shared<cv::Mat>(cv::Mat(width, height, 0));
        buildResponseLayer();
      }

      public:
      std::shared_ptr<cv::Mat> data;

      public:
      int getFilterSize() const { return _filterSize; }
      int getStep() const { return _step; }

      private:
      void buildResponseLayer();
      double getBoxIntegral(int row, int col, int rows, int cols);

      private:
      int _filterSize;
      int _step;

    };
  }
}
