//
// Created by leo on 4/19/17.
//

#include <processing/processing.hpp>
#include "hessian.hpp"
#include "response-map.hpp"


namespace ptc {
  namespace surf {

    void Hessian::getInterestPoints(cv::Mat &img, std::vector<InterestPoint> &iPoints)
    {
      cv::Mat copy(img.size(), img.type());
      cv::integral(img, copy);
      ResponseMap rm(copy, _nbOctaves, _intervalsPerOctave);
      rm.printResponseInfo();
      std::shared_ptr<ResponseLayer> b, m, t;
      for (int i = 0; i < _nbOctaves; i++) {
        for (int j = 0; j + 2 < _intervalsPerOctave; j++) {
          b = rm.layers[i * _intervalsPerOctave + j];
          m = rm.layers[i * _intervalsPerOctave + j + 1];
          t = rm.layers[i * _intervalsPerOctave + j + 2];
          for (int r = 0; r < b->data->size().height; r++)
            for (int c = 0; c < b->data->size().width; c++)
              if (isExtremum(r, c, b, m, t))
                interpolateExtremum(r, c, b, m, t, iPoints);
        }
      }
    }

    bool Hessian::isExtremum(int r, int c, std::shared_ptr<ResponseLayer> b, std::shared_ptr<ResponseLayer> m,
                             std::shared_ptr<ResponseLayer> t) {
      // bounds check
      int layerBorder = (t->getFilterSize() + 1) / (2 * t->getStep());
      if (r <= layerBorder || r >= t->data->size().height - layerBorder || c <= layerBorder
          || c >= t->data->size().width - layerBorder)
        return 0;

      // check the candidate point in the middle layer is above thresh
      int mtscale = m->data->size().width / t->data->size().width;
      float candidate = m->data->at<uchar>(mtscale * r, mtscale * c);
      float thres = 0.004;
      if (candidate < thres)
        return 0;

      int btscale = b->data->size().width / t->data->size().width;

      for (int rr = -1; rr <=1; ++rr)
        for (int cc = -1; cc <=1; ++cc) {
          // if any response in 3x3x3 is greater candidate not maximum
          if (t->data->at<uchar>(r+rr, c+cc) >= candidate ||
              ((rr != 0 || cc != 0) && m->data->at<uchar>((r+rr) * mtscale, (c+cc) * mtscale) >= candidate) ||
              b->data->at<uchar>((r+rr) * btscale, (c+cc) * btscale) >= candidate)
            return 0;
        }

      return 1;
    }

    //! Interpolate scale-space extrema to subpixel accuracy to form an image feature.
    void Hessian::interpolateExtremum(int r, int c, std::shared_ptr<ResponseLayer> b, std::shared_ptr<ResponseLayer> m,
                                      std::shared_ptr<ResponseLayer> t, std::vector<InterestPoint> &featurePoints)
    {
      // get the step distance between filters
      // check the middle filter is mid way between top and bottom
      int filterStep = (m->getFilterSize() - b->getFilterSize());
      assert(filterStep > 0 && t->getFilterSize() - m->getFilterSize() == m->getFilterSize() - b->getFilterSize());

      // Get the offsets to the actual location of the extremum
      double xi = 0, xr = 0, xc = 0;
      interpolateStep(r, c, t, m, b, &xi, &xr, &xc );

      // If point is sufficiently close to the actual extremum
      if( fabs(xi) < 0.5f  &&  fabs(xr) < 0.5f  &&  fabs(xc) < 0.5f )
      {
        InterestPoint ipt(
            static_cast<int>((c + xc) * t->getStep()),
            static_cast<int>((r + xr) * t->getStep()),
            static_cast<float>((0.1333f) * (m->getStep() + xi * filterStep)),
            static_cast<int>(m->getLaplacian(r,c,t))
        );
        featurePoints.push_back(ipt);
      }
    }

//-------------------------------------------------------

//! Performs one step of extremum interpolation.
    void Hessian::interpolateStep(int r, int c, std::shared_ptr<ResponseLayer> b, std::shared_ptr<ResponseLayer> m,
                                  std::shared_ptr<ResponseLayer> t,
                                  double* xi, double* xr, double* xc )
    {
      CvMat* dD, * H, * H_inv, X;
      double x[3] = { 0 };

      dD = deriv3D( r, c, t, m, b );
      H = hessian3D( r, c, t, m, b );
      H_inv = cvCreateMat( 3, 3, CV_64FC1 );
      cvInvert( H, H_inv, CV_SVD );
      cvInitMatHeader( &X, 3, 1, CV_64FC1, x, CV_AUTOSTEP );
      cvGEMM( H_inv, dD, -1, NULL, 0, &X, 0 );

      cvReleaseMat( &dD );
      cvReleaseMat( &H );
      cvReleaseMat( &H_inv );

      *xi = x[2];
      *xr = x[1];
      *xc = x[0];
    }

//-------------------------------------------------------

    //! Computes the partial derivatives in x, y, and scale of a pixel.
    CvMat* Hessian::deriv3D(int r, int c, std::shared_ptr<ResponseLayer> b, std::shared_ptr<ResponseLayer> m,
                            std::shared_ptr<ResponseLayer> t)
    {
      CvMat* dI;
      double dx, dy, ds;

      dx = (m->getDataAtScaled(r, c + 1, t) - m->getDataAtScaled(r, c - 1, t)) / 2.0;
      dy = (m->getDataAtScaled(r + 1, c, t) - m->getDataAtScaled(r - 1, c, t)) / 2.0;
      ds = (t->getDataAt(r, c) - b->getDataAtScaled(r, c, t)) / 2.0;

      dI = cvCreateMat( 3, 1, CV_64FC1 );
      cvmSet( dI, 0, 0, dx );
      cvmSet( dI, 1, 0, dy );
      cvmSet( dI, 2, 0, ds );

      return dI;
    }

//-------------------------------------------------------

//! Computes the 3D Hessian matrix for a pixel.
    CvMat* Hessian::hessian3D(int r, int c, std::shared_ptr<ResponseLayer> b, std::shared_ptr<ResponseLayer> m,
                              std::shared_ptr<ResponseLayer> t)
    {
      CvMat* H;
      double v, dxx, dyy, dss, dxy, dxs, dys;

      v = m->getDataAtScaled(r, c, t);
      dxx = m->getDataAtScaled(r, c + 1, t) + m->getDataAtScaled(r, c - 1, t) - 2 * v;
      dyy = m->getDataAtScaled(r + 1, c, t) + m->getDataAtScaled(r - 1, c, t) - 2 * v;
      dss = t->getDataAt(r, c) + b->getDataAtScaled(r, c, t) - 2 * v;
      dxy = ( m->getDataAtScaled(r + 1, c + 1, t) - m->getDataAtScaled(r + 1, c - 1, t) -
              m->getDataAtScaled(r - 1, c + 1, t) + m->getDataAtScaled(r - 1, c - 1, t) ) / 4.0;
      dxs = ( t->getDataAt(r, c + 1) - t->getDataAt(r, c - 1) -
              b->getDataAtScaled(r, c + 1, t) + b->getDataAtScaled(r, c - 1, t) ) / 4.0;
      dys = ( t->getDataAt(r + 1, c) - t->getDataAt(r - 1, c) -
              b->getDataAtScaled(r + 1, c, t) + b->getDataAtScaled(r - 1, c, t) ) / 4.0;

      H = cvCreateMat( 3, 3, CV_64FC1 );
      cvmSet( H, 0, 0, dxx );
      cvmSet( H, 0, 1, dxy );
      cvmSet( H, 0, 2, dxs );
      cvmSet( H, 1, 0, dxy );
      cvmSet( H, 1, 1, dyy );
      cvmSet( H, 1, 2, dys );
      cvmSet( H, 2, 0, dxs );
      cvmSet( H, 2, 1, dys );
      cvmSet( H, 2, 2, dss );

      return H;
    }

  }
}

