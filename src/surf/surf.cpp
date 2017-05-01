//
// Created by leo on 4/19/17.
//

#include <cmath>
#include <vector>
#include <iostream>
#include <processing/processing.hpp>
#include <unistd.h>
#include "surf.hpp"
#include "interest-point.hpp"
#include "hessian.hpp"
#include "utils.hpp"

namespace ptc {

  namespace surf {

    void Surf::launch() {
      Hessian hessian(4, 4, 2, 400);
      hessian.getInterestPoints(_img, _interestPoints);
      std::cout << "Features points detected: " << _interestPoints.size() << std::endl;
      computeOrientations();
      Utils::drawFeaturePoints(_img, _interestPoints);
    }

    void Surf::computeOrientations() {
      for (auto &e : _interestPoints) {
        float gauss = 0.f;
        float scale = e.getScale();
        const int s = (int) floor(scale + 0.5f), r = (int) floor(e.getY() + 0.5f), c = (int) floor(e.getX() + 0.5f);

        std::vector<float> resX(109), resY(109), Ang(109);
        const int id[] = {6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6};

        int idx = 0;
        for (int i = -6; i <= 6; ++i) {
          for (int j = -6; j <= 6; ++j) {
            if (i * i + j * j < 36) {
              gauss = static_cast<float>(gauss25[id[i + 6]][id[j + 6]]);
              resX[idx] = gauss * haarX(r + j * s, c + i * s, 4 * s);
              resY[idx] = gauss * haarY(r + j * s, c + i * s, 4 * s);
              Ang[idx] = getAngle(resX[idx], resY[idx]);
              ++idx;
            }
          }
        }

        // calculate the dominant direction
        float sumX = 0.f, sumY = 0.f;
        float max = 0.f, orientation = 0.f;
        float ang1 = 0.f, ang2 = 0.f;

        // loop slides pi/3 window around feature point
        for (ang1 = 0; ang1 < 2 * M_PI; ang1 += 0.15f) {
          ang2 = (float) (ang1 + M_PI / 3.0f > 2 * M_PI ? ang1 - 5.0f * M_PI / 3.0f : ang1 + M_PI / 3.0f);
          sumX = sumY = 0.f;
          for (unsigned int k = 0; k < Ang.size(); ++k) {
            // get angle from the x-axis of the sample point
            const float &ang = Ang[k];
            // determine whether the point is within the window
            if (ang1 < ang2 && ang1 < ang && ang < ang2) {
              sumX += resX[k];
              sumY += resY[k];
            }
            else if (ang2 < ang1 && ((ang > 0 && ang < ang2) || (ang > ang1 && ang < 2 * M_PI))) {
              sumX += resX[k];
              sumY += resY[k];
            }
          }

          // if the vector produced from this window is longer than all
          // previous vectors then this forms the new dominant direction
          if (sumX * sumX + sumY * sumY > max) {
            // store largest orientation
            max = sumX * sumX + sumY * sumY;
            orientation = getAngle(sumX, sumY);
          }
        }

        // assign orientation of the dominant response vector
        e.setOrientation(orientation);
      }
    }

//! Calculate Haar wavelet responses in x direction
    float Surf::haarX(int row, int column, int s)
    {
      return (float) (processing::getBoxIntegral(_img, row - s / 2, column, s, s / 2)
                      -1 * processing::getBoxIntegral(_img, row-s/2, column-s/2, s, s/2));
    }

//-------------------------------------------------------

//! Calculate Haar wavelet responses in y direction
    float Surf::haarY(int row, int column, int s)
    {
      return (float) (processing::getBoxIntegral(_img, row, column - s / 2, s / 2, s)
                      -1 * processing::getBoxIntegral(_img, row-s/2, column-s/2, s/2, s));
    }

    float Surf::getAngle(float X, float Y)
    {
      if(X > 0 && Y >= 0)
        return (float) atan(Y / X);
      if(X < 0 && Y >= 0)
        return (float) (M_PI - atan(-Y / X));
      if(X < 0 && Y < 0)
        return (float) (M_PI + atan(Y / X));
      if(X > 0 && Y < 0)
        return (float) (2 * M_PI - atan(-Y / X));
      return 0;
    }

  }

}