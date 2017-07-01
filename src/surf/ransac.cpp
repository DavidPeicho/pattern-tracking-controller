//
// Created by leo on 5/21/17.
//

#include <opencv2/core.hpp>
#include <iostream>
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
      int max = 0;
      int maxIter = (int)(cv::log(1 - probaNotAllOutliers) / cv::log(1 - probaAllPointsGood) + 1);
      std::vector<int> indices(object.size());
      std::iota(indices.begin(), indices.end(), 1);


      for (int i = 0; i < maxIter; ++i) {
        std::random_shuffle(indices.begin(), indices.end());

        // Fill coefs matrix for calculating H
        std::vector<cv::Point2f> in_points;
        std::vector<cv::Point2f> out_points;
        for (unsigned int k = 0; k < nbPoints; k++) {
          in_points.push_back(object[indices[k]]);
          out_points.push_back(scene[indices[k]]);
        }
        // Compute HCandidate the homography matrix
        cv::Mat_<double> HCandidate = cv::Mat_<double>::zeros(3, 3);
        computeModel(in_points, out_points, HCandidate);
        in_points.clear();
        out_points.clear();

        for (unsigned int k = nbPoints; k < indices.size(); k++) {
          in_points.push_back(object[indices[k]]);
          out_points.push_back(scene[indices[k]]);
        }
        // Count transformation that achieve a reasonable error using HCandidate
        int candidate = computeFittingPoints(in_points, out_points, HCandidate, decisionThreshold);

        if (candidate > max) {
          max = candidate;
        }
        // We stop if enough points are calculated correctly from the given H
        if (max >= acceptableNbPoints) {
          H = HCandidate;
          break;
        }
      }
    }


    void Ransac::computeModel(std::vector<cv::Point2f> &in_points, std::vector<cv::Point2f> &out_points,
                              cv::Mat_<double> &H)
    {
      int nbPoints = (int) in_points.size();
      cv::Mat_<double> coefs = cv::Mat_<double>::zeros(nbPoints, 10);
      for (int i = 0; i < nbPoints; i++) {
        cv::Mat_<double> U(3, 1);
        U[0][0] = in_points[i].x;
        U[1][0] = in_points[i].y;
        U[2][0] = 1;
        cv::Mat_<double> V(1, 3);
        V[0][0] = out_points[i].x;
        V[0][1] = out_points[i].y;
        V[0][2] = 1;
        cv::Mat_<double> UV = U * V;
        for (int j = 0; j < 9; j++)
          coefs[i][j] = UV[j / 3][j % 3];
      }
      for (int i = 0; i < nbPoints; i++)
        coefs[i][9] = 0;
      gaussJordan(coefs, H);
    }


    void Ransac::gaussJordan(cv::Mat_<double> &coeffs, cv::Mat_<double> &H) {
      int r = -1;
      for (int j = 0; j < 8; j++) {
        double maxLine = 0;
        // Get pivot line
        int k = 0;
        for (int it = r + 1; it < coeffs.size().height; it++) {
          if (coeffs[it][j] > maxLine) {
            maxLine = coeffs[it][j];
            k = it;
          }
        }
        if (coeffs[k][j] != 0) {
          r++;
          for (int it = 0; it < 10; it++)
            coeffs[k][it] /= coeffs[k][j];
          for (int it = 0; it < 10; it++)
            cv::swap(coeffs[r][it], coeffs[k][it]);
          for (int i = 0; i < coeffs.size().height; i++) {
            if (i != r) {
              for (int it = 0; it < 10; it++)
                coeffs[i][it] -= coeffs[r][it] * coeffs[i][j];
            }
          }
        }
        std::cout << "coeffs" << std::endl;
        for (int i = 0; i < 8; i++) {
          for (int l = 0; l < 10; l++)
            std::cout << coeffs[i][l] << " ";
          std::cout << std::endl;
        }
        std::cout << std::endl;
      }
      for (int i = 0; i < 9; i++) {
        H[i / 3][i % 3] = coeffs[i][8];
      }
    }

    int Ransac::computeFittingPoints(std::vector<cv::Point2f> &in_points, std::vector<cv::Point2f> &out_points,
                                     cv::Mat_<double> &H, double decisionThreshold) {
      int nbGood = 0;
      int nbPoints = (int) in_points.size();
      for (int k = 0; k < nbPoints; k++) {
        cv::Mat_<double> U(3, 1);
        U[0][0] = in_points[k].x;
        U[1][0] = in_points[k].y;
        U[2][0] = 1;
        cv::Mat_<double> V(1, 3);
        V[0][0] = out_points[k].x;
        V[0][1] = out_points[k].y;
        V[0][2] = 1;
        double normV = cv::norm(V);
        cv::Mat_<double> UH = H * U;
        double normUH = cv::norm(UH);
        double err = cv::abs(normUH - normV);
        if (err < decisionThreshold)
           nbGood += 1;
      }
      return nbGood;
    }

  }
}
