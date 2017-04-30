//
// Created by leo on 4/24/17.
//

#pragma once 

#include <vector>
#include "response-layer.hpp"
#include <memory>

namespace ptc {
  namespace surf {

    class ResponseMap {
      public:
      ResponseMap(cv::Mat &img, int nbOctaves, int nbIntervals);

      public:
      void printResponseInfo();

      public:
      std::vector<std::shared_ptr<ResponseLayer>> layers;

      private:
      int _nbOctaves;
      int _nbIntervals;

    };

  }
}
