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

}

}

}