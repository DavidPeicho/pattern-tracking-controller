#include <processing/convolution.hpp>

namespace ptc {

namespace processing {

namespace conv {

KernelMat::KernelMat(std::vector<int> kernel, size_t size, double factor)
          : kernel_{kernel}
          , size_{size}
          , factor_{factor}
{ }

int&
KernelMat::at(int y, int x) {

  return kernel_[size_ * y + x];

}

void
applyConvolution(cv::Mat& output, cv::Mat& input, KernelMat& kernel) {

  int half = (int)(kernel.getSize() / 2);

  double factor = kernel.getFactor();
  int sum = 0;

  for (int y = 0; y < input.rows; ++y) {

    for (int x = 0; x < input.cols; ++x) {

      for (int yy = - half; yy <= half; ++yy) {

        for (int xx = - half; xx <= half; ++xx) {
          if (x + xx < 0 || y + yy < 0 ||
              x + xx >= input.cols || y + yy >= input.rows) {
            continue;
          }
          uint8_t& val = input.at<uint8_t>(y + yy, x + xx);
          sum += val * kernel.at(yy + half, xx + half);
        }
      }

      int pxValue = (int)(sum * factor);
      output.at<uchar>(y, x) = (uint8_t)utils::maths::clamp(pxValue, 0, 255);

      sum = 0;
    }
  }

}

void
applyBiconvolution(cv::Mat& output, cv::Mat& input,
                   KernelMat& kernelA,
                   KernelMat& kernelB,
                   double (*biconvolution) (double a, double b)) {

  int half = (int)(kernelA.getSize() / 2);

  double factor = kernelA.getFactor();
  int valA = 0;
  int valB = 0;

  for (int y = 0; y < input.rows; ++y) {

    for (int x = 0; x < input.cols; ++x) {

      for (int yy = - half; yy <= half; ++yy) {

        for (int xx = - half; xx <= half; ++xx) {
          if (x + xx < 0 || y + yy < 0 ||
              x + xx >= input.cols || y + yy >= input.rows) {
            continue;
          }
          uint8_t& val = input.at<uint8_t>(y + yy, x + xx);

          valA += val * kernelA.at(yy + half, xx + half);
          valB += val * kernelA.at(yy + half, xx + half);
        }
      }

      //int pxValue = (int)(sum * factor);
      int pxValue = (int)biconvolution(valA * factor, valB * factor);
      output.at<uchar>(y, x) = (uint8_t)utils::maths::clamp(pxValue, 0, 255);

      valA = 0;
      valB = 0;
    }
  }

}

/*void
applyCanny(cv::Mat& output, cv::Mat& input) {

  //(utils::Logger::instance()*) << "Begin canny computing on ";

  int h = output.size().height;
  int w = output.size().width;

  // Begins by removing noise from the original image
  cv::Mat smoothed = cv::Mat::zeros(h, w, CV_8UC1);
  processing::conv::applyConvolution(smoothed, input, GAUSSIAN5_K);

  cv::Mat sobelX = cv::Mat::zeros(h, w, CV_8UC1);
  cv::Mat sobelY = cv::Mat::zeros(h, w, CV_8UC1);
  cv::Mat g = cv::Mat::zeros(h, w, CV_8UC1);

  applyConvolution(sobelX, smoothed, SOBELX_K);
  applyConvolution(sobelY, smoothed, SOBELY_K);

  for (int y = 0; y < input.rows; ++y) {
    for (int x = 0; x < input.cols; ++x) {
      uint8_t sx = sobelX.at<uint8_t>(y, x);
      uint8_t sy = sobelY.at<uint8_t>(y, x);
      g.at<uint8_t>(y, x) = (uint8_t) hypot(sx, sy);
    }
  }

  // Non-maximum suppression
  for (int y = 1; y < h - 1; y++) {
    for (int x = 1; x < w - 1; x++) {
      auto val = g.at<uint8_t>(y, x);

      auto sx = sobelX.at<uint8_t>(y, x);
      auto sy = sobelY.at<uint8_t>(y, x);

      const float dir =
        (float) (fmod(atan2(sy, sx) + M_PI, M_PI) / M_PI) * 8.0f;

      auto nn = g.at<uint8_t>(y - 1, x);
      auto ss = g.at<uint8_t>(y + 1, x);
      auto ee = g.at<uint8_t>(y, x - 1);
      auto ww = g.at<uint8_t>(y, x + 1);
      auto nw = g.at<uint8_t>(y - 1, x + 1);
      auto ne = g.at<uint8_t>(y - 1, x - 1);
      auto sw = g.at<uint8_t>(y + 1, x + 1);
      auto se = g.at<uint8_t>(y + 1, x - 1);

      if (((dir <= 1.0 || dir > 7.0) && val > ee &&
           val > ww) ||
          ((dir > 1 && dir <= 3) && val > nw &&
           val > se) ||
          ((dir > 3 && dir <= 5) && val > nn &&
           val > ss) ||
          ((dir > 5 && dir <= 7) && val > ne &&
           val > sw))
        output.at<uint8_t>(y, x) = val;
      else
        output.at<uint8_t>(y, x) = 0;
    }
  }

}*/

}

}

}