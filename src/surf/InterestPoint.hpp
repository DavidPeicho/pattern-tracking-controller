//
// Created by leo on 4/30/17.
//

#pragma once 


class InterestPoint {
  public:
  InterestPoint(int x, int y, float scale, int laplacian) : _x(x), _y(y), _scale(scale), _laplacian(laplacian) {}

  public:
  int getX() const { return _x; }
  int getY() const { return _y; }
  float getScale() const { return _scale; }
  int getLaplacian() const { return _laplacian; }

  private:
  int _x;
  int _y;
  float _scale;
  int _laplacian;
};


