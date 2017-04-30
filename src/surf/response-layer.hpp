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
        laplacian = std::make_shared<cv::Mat>(cv::Mat(width, height, 0));
        buildResponseLayer();
      }

      public:
      inline unsigned char getLaplacian(int r, int c, std::shared_ptr<ResponseLayer> other)
      {
        int scale = data->size().width / other->data->size().width;
        return laplacian->at<uchar>(scale * r, scale * c);
      }

      public:
      std::shared_ptr<cv::Mat> data;
      std::shared_ptr<cv::Mat> laplacian;

      public:
      int getFilterSize() const { return _filterSize; }
      int getStep() const { return _step; }
      uchar getDataAt(int r, int c) const { return data->at<uchar>(r, c); }
      uchar getDataAtScaled(int r, int c, std::shared_ptr<ResponseLayer> other) const {
        int scale = data->size().width / other->data->size().width;
        return data->at<uchar>(r * scale, c * scale);
      }

      private:
      void buildResponseLayer();
      double getBoxIntegral(int row, int col, int rows, int cols);

      private:
      int _filterSize;
      int _step;

    };
  }
}
