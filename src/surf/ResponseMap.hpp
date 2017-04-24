//
// Created by leo on 4/24/17.
//

#pragma once 

#include <vector>
#include "Octave.hpp"
#include <memory>

namespace ptc {
  namespace surf {

    class ResponseMap {
      public:
      ResponseMap(int nbOctaves, int initialStep) : _nbOctaves(nbOctaves), _initialStep(initialStep) {}

      private:
      std::vector<std::shared_ptr<Octave>> octaves;
      int _nbOctaves;
      int _initialStep;

    };

  }
}
