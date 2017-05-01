//
// Created by leo on 4/27/17.
//

#include <processing/processing.hpp>
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
          Dxx = processing::getBoxIntegral(*data, r - lobe + 1, c - band, 2 * lobe - 1, _filterSize)
                - processing::getBoxIntegral(*data, r - lobe + 1, c - lobe / 2, 2 * lobe - 1, lobe) * 3;
          Dyy = processing::getBoxIntegral(*data, r - band, c - lobe + 1, _filterSize, 2 * lobe - 1)
                - processing::getBoxIntegral(*data, r - lobe / 2, c - lobe + 1, lobe, 2 * lobe - 1) * 3;
          Dxy = + processing::getBoxIntegral(*data, r - lobe, c + 1, lobe, lobe)
                + processing::getBoxIntegral(*data, r + 1, c - lobe, lobe, lobe)
                - processing::getBoxIntegral(*data, r - lobe, c - lobe, lobe, lobe)
                - processing::getBoxIntegral(*data, r + 1, c + 1, lobe, lobe);
          Dxx *= nFactor;
          Dyy *= nFactor;
          Dxy *= nFactor;
          data->at<uchar>(r, c) = (uint8_t)(Dxx * Dyy - 0.81 * Dxy * Dxy);
          laplacian->at<uchar>(r, c) = (uint8_t)(Dxx + Dyy >= 0 ? 1 : 0);
        }
    }

  }
}