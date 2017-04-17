#pragma once

#include <opencv2/opencv.hpp>

#include <utils/utils.hpp>

namespace ptc {

namespace processing {

namespace conv {

class KernelMat {

  public:
    KernelMat(std::vector<int> kernel, size_t size, double factor);

  public:
    int&
    at(int y, int x);

    inline size_t
    getSize() {

      return size_;

    }

    inline double
    getFactor() {

      return factor_;

    }

  private:
    std::vector<int>  kernel_;
    double            factor_;
    size_t            size_;

};

static KernelMat GAUSSIAN5_K({ 2, 4, 5, 4, 2,
                               4, 9, 12, 9, 4,
                               5, 12, 15, 12, 5,
                               4, 9, 12, 9, 4,
                               2, 4, 5, 4, 2
                             }, 5, 0.0062);

static KernelMat GAUSSIAN3_K({ 1, 2, 1,
                               2, 4, 2,
                               1, 2, 1
                             }, 3, 0.0625);

static KernelMat SOBELX_K({ 1, 0, -1,
                            2, 0, -2,
                            1, 0, -1
                          }, 3, 1.0);

static KernelMat SOBELY_K({  1,  2,  1,
                             0,  0,  0,
                            -1, -2, -1
                          }, 3, 1.0);

void
applyConvolution(cv::Mat& output, cv::Mat& input, KernelMat& kernel);

void
applyBiconvolution(cv::Mat& output, cv::Mat& input,
                   KernelMat& kernelA,
                   KernelMat& kernelB,
                   double (*biconvolution) (double a, double b));

}

}

}