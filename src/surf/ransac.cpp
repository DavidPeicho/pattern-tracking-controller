//
// Created by leo on 5/21/17.
//

#include <opencv2/core.hpp>
#include <iostream>
#include "ransac.hpp"

namespace ptc {
  namespace surf {
    void Ransac::findHomography(std::vector<cv::Point2f> &object, std::vector<cv::Point2f> &scene, cv::Mat &H) {
      unsigned int nbPoints = 4;
      double probaPointWrong = 0.1;
      double probaPointGood = 1 - probaPointWrong;
      double probaAllPointsGood = cv::pow(probaPointGood, nbPoints);
      double probaNotAllOutliers = 0.99;
      double decisionThreshold = 10;
      int acceptableNbPoints = (int)(object.size() * 0.95);
      int max = 0;
      int maxIter = (int)(cv::log(1 - probaNotAllOutliers) / cv::log(1 - probaAllPointsGood) + 1);
      std::vector<int> indices(object.size());
      std::iota(indices.begin(), indices.end(), 1);
      cv::Mat_<double> HCandidate = cv::Mat_<double>::zeros(3, 3);

      std::vector<cv::Point2f> in_points(nbPoints);
      std::vector<cv::Point2f> out_points(nbPoints);
      std::vector<cv::Point2f> in_points2(object.size() - nbPoints);
      std::vector<cv::Point2f> out_points2(object.size() - nbPoints);

      for (int i = 0; i < 500; ++i) {
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
          H = HCandidate.clone();
          max = candidate;
        }
        // We stop if enough points are calculated correctly from the given H
        if (max >= acceptableNbPoints) {
          break;
        }
      }
      H /= H.at<double>(2, 2);  // Normalize H
    }


    double Ransac::computeModel(const std::vector<cv::Point2f> &in_points, const std::vector<cv::Point2f> &out_points,
                                cv::Mat_<double> &H)
    {
      auto A = getHomographyLinearSystem(in_points, out_points);
      cv::Mat_<double> At;
      cv::transpose(A, At);
      cv::Mat_<double> AtA = At * A;
      cv::Mat_<double> Hest = cv::Mat_<double>(3, 3);
      cv::Mat_<double> U = cv::Mat_<double>::zeros(A.rows, A.rows * A.cols);
      cv::Mat_<double> Vt = cv::Mat_<double>::zeros(A.cols, A.rows * A.cols);
      cv::Mat_<double> S = cv::Mat_<double>::zeros(A.rows * A.cols, 1);
      cv::SVD::compute(AtA, S, U, Vt);  // SVD of A
      cv::Mat_<double> h = Vt.row(Vt.rows - 1); // Last singular vector of least singular value
      for (int i = 0; i < 9; i++) {
        Hest.at<double>(i / 3, i % 3) = h.at<double>(0, i);
      }

      /*
      U = cv::Mat_<double>::zeros(3, 3);
      Vt = cv::Mat_<double>::zeros(3, 3);
      S = cv::Mat_<double>::zeros(3, 1);
      cv::SVD::compute(Hest, S, U, Vt);  // SVD of A
      cv::Mat_<double> Sdiag = cv::Mat_<double>::zeros(3, 3);
      for (int i = 0; i < 2; i++)  // We put last element at zero
        Sdiag.at<double>(i, i) = S.at<double>(i, 0);
      H = U * Sdiag * Vt;
      */
      Hest.copyTo(H);
      double max_err = 0;
      double total_err = 0;
      for (unsigned int i = 0; i < in_points.size(); i++) {
        cv::Mat_<double> X = cv::Mat_<double>(3, 1);
        X.at<double>(0, 0) = in_points[i].x;
        X.at<double>(1, 0) = in_points[i].y;
        X.at<double>(2, 0) = 1;
        cv::Mat_<double> Y = cv::Mat_<double>(3, 1);
        Y.at<double>(0, 0) = out_points[i].x;
        Y.at<double>(1, 0) = out_points[i].y;
        Y.at<double>(2, 0) = 1;
        cv::Mat_<double> HX = H * X;
        HX /= HX.at<double>(2, 0);
        double err = cv::norm(HX - Y);
        if (err > max_err)
          max_err = err;
        total_err += err;
      }
      total_err -= max_err;
      return total_err;
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
      cv::Mat_<double> V = cv::Mat_<double>::zeros(3, 1);
      for (int k = 0; k < nbPoints; k++) {
        U.at<double>(0, 0) = in_points[k].x;
        U.at<double>(1, 0) = in_points[k].y;
        U.at<double>(2, 0) = 1;
        V.at<double>(0, 0) = out_points[k].x;
        V.at<double>(1, 0) = out_points[k].y;
        V.at<double>(2, 0) = 1;
        cv::Mat_<double> UH = H * U;
        UH /= UH.at<double>(2, 0);
        double err = cv::norm(UH - V);
        if (err < decisionThreshold)
          nbGood += 1;
      }
      return nbGood;
    }

    cv::Mat_<double> Ransac::getHomographyLinearSystem(const std::vector<cv::Point2f> &in_points,
                                                       const std::vector<cv::Point2f> &out_points)
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
      return A;
    }

  }
}
