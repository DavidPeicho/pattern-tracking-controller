//
// Created by leo on 4/30/17.
//

#pragma once 


#include <opencv2/core/matx.hpp>

namespace ptc {
  namespace surf {

    class interest {
      public:
      interest(int x, int y, float scale, int laplacian) : _x(x), _y(y), _scale(scale),
                                                                               _laplacian(laplacian) {}

      public:
      int getX() const { return _x; }
      int getY() const { return _y; }
      float getScale() const { return _scale; }
      int getLaplacian() const { return _laplacian; }
      void setOrientation(float orientation) { _orientation = orientation; }
      const float &getOrientation() const { return _orientation; }

      private:
      int _x;
      int _y;
      float _scale;
      int _laplacian;
      float _orientation;

    };


  }
}
