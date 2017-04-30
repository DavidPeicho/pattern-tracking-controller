//
// Created by leo on 4/19/17.
//

#include <processing/processing.hpp>
#include "hessian.hpp"
#include "response-map.hpp"


namespace ptc {
  namespace surf {

    std::vector<cv::Vec2i> Hessian::hessian(cv::Mat &img, int nbOctaves, int intervalsPerOctave, int initSamplingStep) {
      cv::Mat copy(img.size(), img.type());
      processing::integralImage(copy, img);
      std::vector<cv::Vec2i> featurePoints;
      getInterestPoints(img, featurePoints, nbOctaves, intervalsPerOctave, initSamplingStep);
      return featurePoints;
    }

    void Hessian::getInterestPoints(cv::Mat &img, std::vector<cv::Vec2i> &iPoints,
                                    int nbOctaves, int intervalsPerOctave, int initSamplingStep)
    {
      ResponseMap rm(img, nbOctaves, intervalsPerOctave);
      rm.printResponseInfo();
      std::shared_ptr<ResponseLayer> b, m, t;
      for (int i = 0; i < nbOctaves; i++) {
        for (int j = 0; j + 2 < intervalsPerOctave; j++) {
          b = rm.layers[i * intervalsPerOctave + j];
          m = rm.layers[i * intervalsPerOctave + j + 1];
          t = rm.layers[i * intervalsPerOctave + j + 2];
          for (int r = 0; r < b->data->size().height; r++)
            for (int c = 0; c < b->data->size().width; c++)
              if (isExtremum(r, c, b, m, t))
                interpolateExtremum(r, c, b, m, t);
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

    void Hessian::interpolateExtremum(int r, int c, std::shared_ptr<ResponseLayer> b, std::shared_ptr<ResponseLayer> m,
                                      std::shared_ptr<ResponseLayer> t) {

    }


  }
}

