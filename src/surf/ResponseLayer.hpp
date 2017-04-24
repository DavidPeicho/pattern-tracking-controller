//
// Created by leo on 4/24/17.
//

#pragma once 

#include <opencv2/core/mat.hpp>

class ResponseLayer {
  public:
  ResponseLayer(int width, int height, int filterSize) : data(width, height), _filterSize(filterSize) {}

  public:
  cv::Mat data;

  private:
  int _filterSize;

};


