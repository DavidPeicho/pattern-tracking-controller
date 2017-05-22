//
// Created by leo on 5/21/17.
//

#include <opencv2/core.hpp>
#include "ransac.hpp"

namespace ptc {
  namespace surf {
    void Ransac::findHomography(std::vector<cv::Point2f> &object, std::vector<cv::Point2f> &scene, cv::Mat &H) {
      unsigned int nbPoints = 8;
      double probaPointWrong = 0.1;
      double probaPointGood = 1 - probaPointWrong;
      double probaAllPointsGood = cv::pow(probaPointGood, nbPoints);
      double probaNotAllOutliers = 0.99;
      float decisionThreshold = 0.1;
      int acceptableNbPoints = (int)(object.size() * 0.95);
      std::vector<int> indices(nbPoints);
      std::iota(indices.begin(), indices.end(), 0);
      cv::Mat_<float> bestHCandidate = cv::Mat_<float>::zeros(3, 3);

      int max = 0;
      int candidate = 0;
      cv::Mat_<float> HCandidate = cv::Mat_<float>::zeros(3, 3);
      int maxIter = (int)(cv::log(1 - probaNotAllOutliers) / cv::log(1 - probaAllPointsGood) + 1);
      for (int i = 0; i < maxIter; ++i) {
        std::random_shuffle(indices.begin(), indices.end());

        // Fill coefs matrix for calculating H
        cv::Mat_<double> coefs = cv::Mat_<float>::zeros(nbPoints, 9);
        for (int k = 0; k < nbPoints; k++) {
          cv::Mat_<double> U(3, 1);
          U[0][0] = object[indices[k]].x;
          U[1][0] = object[indices[k]].y;
          U[2][0] = 1;
          cv::Mat_<double> V(1, 3);
          V[0][0] = scene[indices[k]].x;
          V[1][0] = scene[indices[k]].y;
          V[2][0] = 1;
          cv::Mat_<double> UV(3, 3) = U * V;
          for (int h = 0; h < 9; h++)
            coefs[k][h] = UV[h / 3][h % 3];
        }
        gaussJordan(coefs, HCandidate);

        // Compute model
        for (int k = nbPoints; k < indices.size(); k++) {
          // Test if it fits the model
        }

        max = (candidate > max) ? candidate : max;
        if (max >= acceptableNbPoints)
          break;
        candidate = 0;
      }
    }

    void Ransac::gaussJordan(cv::Mat_<double> &coeffs, cv::Mat_<float> &H) {
      int r = 0;
      for (int j = 0; j < 9; j++) {
        double maxLine = 0;
        // Get pivot line
        int k = 0;
        for (k = r + 1; k < coeffs.size().height; j++) {
          if (coeffs[k][j] > maxLine) {
            maxLine = coeffs[k][j];
            break;
          }
        }
        if (coeffs[k][j] != 0) {
          r++;
          for (auto& e : coeffs.row(k))
            e /= coeffs[k][j];
          for (int it = 0; it < 9; it++)
            cv::swap(coeffs[r][it], coeffs[k][it]);
          for (int i = 0; i < coeffs.size().height; i++) {
            if (i != r) {
              for (int it = 0; it < 9; it++)
                coeffs[i][it] -= coeffs[r][it];
            }
          }
        }
      }
    }

  }
}
