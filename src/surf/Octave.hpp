//
// Created by leo on 4/24/17.
//

#pragma once

#include "response-layer.hpp"
#include <memory>
#include <vector>

namespace ptc {
  namespace surf {

    class Octave {
      public:
      Octave(int width, int height, int step, int initialFilterSize) :
        _width(width), _height(height), _step(step), _initialFiltreSize(initialFilterSize) {}

      public:
      std::vector<std::shared_ptr<ResponseLayer>> layers;

      private:
      int _width;
      int _height;
      int _step;
      int _initialFiltreSize;
    };

  }
}


