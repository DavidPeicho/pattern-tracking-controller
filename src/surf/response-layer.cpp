//
// Created by leo on 4/27/17.
//

#include "response-layer.hpp"

namespace ptc {
  namespace surf {
    void ResponseLayer::buildResponseLayer() {
      int lobe = _filterSize / 3;
      int band = _filterSize / 9 * 5;
      double Dxx, Dxy, Dyy;
      double nFactor = 1 / (_filterSize * _filterSize);
      for (int j = 0; j * _step < data->size().height; j++)
        for (int i = 0; i * _step < data->size().width; i++ ) {
          int r = j * _step;
          int c = i * _step;
          Dxx = getBoxIntegral(r - lobe + 1, c - band, 2 * lobe - 1, _filterSize)
                - getBoxIntegral(r - lobe + 1, c - lobe / 2, 2 * lobe - 1, lobe) * 3;
          Dyy = getBoxIntegral(r - band, c - lobe + 1, _filterSize, 2 * lobe - 1)
                - getBoxIntegral(r - lobe / 2, c - lobe + 1, lobe, 2 * lobe - 1) * 3;
          Dxy = + getBoxIntegral(r - lobe, c + 1, lobe, lobe)
                + getBoxIntegral(r + 1, c - lobe, lobe, lobe)
                - getBoxIntegral(r - lobe, c - lobe, lobe, lobe)
                - getBoxIntegral(r + 1, c + 1, lobe, lobe);
          Dxx *= nFactor;
          Dyy *= nFactor;
          Dxy *= nFactor;
          data->at<uchar>(r, c) = (uint8_t)(Dxx * Dyy - 0.81 * Dxy * Dxy);
          laplacian->at<uchar>(r, c) = (uint8_t)(Dxx + Dyy >= 0 ? 1 : 0);
        }
    }

    double ResponseLayer::getBoxIntegral(int row, int col, int rows, int cols) {
      float A = 0, B = 0, C = 0, D = 0;
      int r1 = std::min(row, data->size().height - 1);
      int c1 = std::min(row, data->size().width - 1);
      int r2 = std::min(row + rows, data->size().height - 1);
      int c2 = std::min(col + cols, data->size().width - 1);
      if (r1 >= 0 && c1 >= 0) A = data->at<uchar>(r1, c1);
      if (r1 >= 0 && c2 >= 0) B = data->at<uchar>(r1, c2);
      if (r2 >= 0 && c1 >= 0) C = data->at<uchar>(r2, c1);
      if (r2 >= 0 && c2 >= 0) D = data->at<uchar>(r2, c2);
      return std::max(0.f, A - B - C + D);
    }


  }
}