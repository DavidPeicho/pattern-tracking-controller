#include <processing/convolution.hpp>

namespace ptc {

namespace processing {

void
convert(cv::Mat& output, cv::Mat& input) {

  for (int y = 0; y < input.rows; ++y) {
    for (int x = 0; x < input.cols; ++x) {
      auto& px = input.at<cv::Vec3b>(y, x);
      uint8_t value = (uint8_t)(0.299 * px[2] + 0.587 * px[1] + 0.114 * px[0]);
      output.at<uchar>(y, x) = value;
    }
  }

}

}

}