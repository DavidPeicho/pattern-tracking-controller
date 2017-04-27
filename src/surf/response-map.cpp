#include <iostream>
#include "response-map.hpp"

//
// Created by leo on 4/25/17.
//

namespace ptc {
  namespace surf {

    ResponseMap::ResponseMap(cv::Mat &img, int nbOctaves, int nbIntervals) :
      _nbOctaves(nbOctaves), _nbIntervals(nbIntervals)
    {
      int filterSize = 9;
      int filterSizeIncrease = 6;
      int w = img.size().width;
      int h = img.size().height;
      int step = 2;
      if (nbOctaves >= 1) {
        for (int i = 0; i < nbIntervals; ++i) {
          layers.push_back(std::make_shared<ResponseLayer>(filterSize, w, h, step));
          filterSize += filterSizeIncrease;
        }
      }
      int duplicateIndex = 1;
      int previousSize = nbIntervals;
      int added = 0;
      for (int i = 1; i < nbOctaves; ++i) {
        filterSizeIncrease *= 2;
        w /= 2;
        h /= 2;
        step *= 2;
        for (duplicateIndex = previousSize - nbIntervals + 1; duplicateIndex < previousSize; duplicateIndex += 2) {
          layers.push_back(layers[duplicateIndex]);
          added++;
        }
        for (added; added < nbIntervals; added++) {
          layers.push_back(std::make_shared<ResponseLayer>(filterSize, w, h, step)); // TODO: verify filter size
          filterSize += filterSizeIncrease;
        }
        added = 0;
        previousSize += nbIntervals;
      }
    }

    void ResponseMap::printResponseInfo() {
      int i = 0;
      int octaveNb = 0;
      for (auto e : layers) {
        if (i == _nbIntervals) {
          i = 0;
          octaveNb++;
        }
        std::cout << "Layer " << octaveNb << " : filter size " << e->getFilterSize()
          << ", w = " << e->data->size().width
          << ", h = " << e->data->size().height
          << ", step = " << e->getStep()
          << std::endl;
        i++;
      }
    }


  }
}
