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
      cv::Mat_<double> HCandidate = cv::Mat_<double>::zeros(3, 3);

      std::vector<cv::Point2f> in_points(8);
      std::vector<cv::Point2f> out_points(8);
      std::vector<cv::Point2f> in_points2(24);
      std::vector<cv::Point2f> out_points2(24);

      for (int i = 0; i < maxIter; ++i) {
        std::random_shuffle(indices.begin(), indices.end());

        int kmax = nbPoints;
        int it = 0;
        for (int k = 0; k < kmax; k++) {
          int l = indices[k];
          // Ignore points if they have a small value (artifacts of previous computations)
          if (object[l].x < 0.1 || object[l].y < 0.1 || scene[l].x < 0.1 || scene[l].y < 0.1) {
            kmax++;
            continue;
          }
          in_points[it] = object[l];
          out_points[it] = scene[l];
          it++;
        }
        // Compute HCandidate the homography matrix
        computeModel(in_points, out_points, HCandidate);

        it = 0;
        for (unsigned int k = nbPoints; k < object.size(); k++) {
          int l = indices[k];
          in_points2[it] = object[l];
          out_points2[it] = scene[l];
          it++;
        }
        // Count transformation that achieve a reasonable error using HCandidate
        int candidate = computeFittingPoints(in_points2, out_points2, HCandidate, decisionThreshold);

        if (candidate > max) {
          max = candidate;
        }
        // We stop if enough points are calculated correctly from the given H
        if (max >= acceptableNbPoints) {
          H = HCandidate.clone();
          break;
        }
      }
    }


    void Ransac::computeModel(const std::vector<cv::Point2f> &in_points, const std::vector<cv::Point2f> &out_points,
                              cv::Mat_<double> &H)
    {
      int nbPoints = (int) in_points.size();
      cv::Mat_<double> A = cv::Mat_<double>::zeros(nbPoints * 2, 9);
      for (int i = 0; i < nbPoints; i++) {
        A.at<double>(2 * i, 0) = in_points[i].x;
        A.at<double>(2 * i, 1) = in_points[i].y;
        A.at<double>(2 * i, 2) = 1;
        A.at<double>(2 * i, 3) = A.at<double>(2 * i, 4) = A.at<double>(2 * i, 5) = 0;
        A.at<double>(2 * i, 6) = -out_points[i].x * in_points[i].x;
        A.at<double>(2 * i, 7) = -out_points[i].x * in_points[i].y;
        A.at<double>(2 * i, 8) = -out_points[i].x;
        A.at<double>(2 * i + 1, 0) = A.at<double>(2 * i + 1, 1) = A.at<double>(2 * i + 1, 0) = 0;
        A.at<double>(2 * i + 1, 3) = in_points[i].x;
        A.at<double>(2 * i + 1, 4) = in_points[i].y;
        A.at<double>(2 * i + 1, 5) = 1;
        A.at<double>(2 * i + 1, 6) = -out_points[i].y * in_points[i].x;
        A.at<double>(2 * i + 1, 7) = -out_points[i].y * in_points[i].y;
        A.at<double>(2 * i + 1, 8) = -out_points[i].y;
      }
      cv::Mat_<double> At = cv::Mat_<double>::zeros(9, nbPoints * 2);
      cv::transpose(A, At);
      cv::Mat_<double> AA = At * A;
      cv::Mat_<double> eigenvectors = cv::Mat_<double>::zeros(9, 9);
      std::vector<double> eigenvalues(9);
      cv::eigen(AA, eigenvalues, eigenvectors);
      int l = (int) (eigenvalues.size() - 1); // The index of the eigenvect with smallest eigenval
      for (int i = 0; i < 9; i++) {
        eigenvectors.at<double>(l, i) /= eigenvectors.at<double>(l, 8);
        H.at<double>(i / 3, i % 3) = eigenvectors.at<double>(l, i);
      }
      /*
      std::cout << "FOUND HOMOGRAPHY:" << std::endl;
      printMat(H);
      */
      /*
      cv::Mat_<double> eigenV = cv::Mat_<double>::zeros(9, 1);
      for (int i = 0; i < 9; i++) {
        eigenV[i][0] = eigenvectors[l][i];
      }
      cv::Mat_<double> zero = A * eigenV;
      printMat(zero);
       */
    }

    void Ransac::printMat(const cv::Mat_<double> &mat) {
      for (int i = 0; i < mat.size().height; i++) {
        for (int j = 0; j < mat.size().width; j++) {
          std::cout << mat.at<double>(i, j) << "  ";
        }
        std::cout << std::endl;
      }
    }

    int Ransac::computeFittingPoints(const std::vector<cv::Point2f> &in_points, const std::vector<cv::Point2f> &out_points,
                                     const cv::Mat_<double> &H, double decisionThreshold) {
      int nbGood = 0;
      int nbPoints = (int) in_points.size();
      cv::Mat_<double> U = cv::Mat_<double>::zeros(3, 1);
      cv::Mat_<double> V = cv::Mat_<double>::zeros(1, 3);
      for (int k = 0; k < nbPoints; k++) {
        U.at<double>(0, 0) = in_points[k].x;
        U.at<double>(1, 0) = in_points[k].y;
        U.at<double>(2, 0) = 1;
        V.at<double>(0, 0) = out_points[k].x;
        V.at<double>(0, 1) = out_points[k].y;
        V.at<double>(0, 2) = 1;
        cv::Mat_<double> UH = H * U;
        cv::transpose(UH, UH);
        for (int i = 0; i < 3; i++) {
          //double a = UH.at<double>(2, 0);
          UH.at<double>(i, 0) /= UH.at<double>(2, 0);
        }
        //std::cout << "UH" << std::endl;
        //printMat(UH);
        //std::cout << "V" << std::endl;
        //printMat(V);
        cv::Mat_<double> diff(UH - V);
        //std::cout << "diff" << std::endl;
        //printMat(diff);
        double err = cv::norm(diff);
        if (err < decisionThreshold)
           nbGood += 1;
      }
      return nbGood;
    }

  }
}
