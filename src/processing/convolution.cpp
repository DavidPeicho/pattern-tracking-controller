#include <processing/convolution.hpp>

namespace ptc {

namespace processing {

int
computeLocalTreshold(cv::Mat& input, int yInit, int xInit, int wSize) {

  // This function implements the Sovola algorithm.
  // TODO: One good optimization would be to use the Integral Images
  // TODO: that brings this algorithm closer to a O(1) algorithm.

  double mean = 0;
  double meanSquare = 0;
  double s = 0;
  int count = 0;
  for (int y = yInit - (wSize / 2); y <= yInit + (wSize / 2); ++y) {
    for (int x = xInit - (wSize / 2); x <= xInit + (wSize / 2); ++x) {
      if ((y >= 0 && x >= 0) && (y < input.rows && x < input.cols)) {
        uint8_t& val = input.at<uchar>(y, x);
        mean += val;
        meanSquare += val * val;
        count++;
      }
    }
  }

  mean /= count;
  meanSquare /= count;

  s = sqrt(meanSquare - mean * mean);

  return (int)(mean * (1 + 0.4 * ((s / 128.0) - 1)));

}

void
binarize(cv::Mat& output, cv::Mat& input) {

  for (int y = 0; y < input.rows; ++y) {
    for (int x = 0; x < input.cols; ++x) {
      uint8_t& val = input.at<uchar>(y, x);
      if (val >= computeLocalTreshold(input, y, x, 20)) {
        output.at<uchar>(y, x) = 255;
      } else {
        output.at<uchar>(y, x) = 0;
      }
    }
  }

}

void
grayscale(cv::Mat& output, cv::Mat& input) {

  for (int y = 0; y < input.rows; ++y) {
    for (int x = 0; x < input.cols; ++x) {
      auto& px = input.at<cv::Vec3b>(y, x);
      uint8_t value = (uint8_t)(0.299 * px[2] + 0.587 * px[1] + 0.114 * px[0]);
      output.at<uchar>(y, x) = value;
    }
  }

}

void
downscaleBy(cv::Mat& output, cv::Mat& input, double factor) {

  int prevX = 0;
  int prevY = 0;
  int nextX = 0;
  int nextY = 0;

  double prevRow = 0.0;
  double nextRow = 0.0;
  double totalLerp = 0.0;

  for (int y = 0; y < output.rows; ++y) {
    for (int x = 0; x < output.cols; ++x) {
      double currX = (double)x * factor;
      double currY = (double)y * factor;
      prevX = utils::maths::clamp((int)floor(currX) - 1, 0, input.cols - 1);
      nextX = utils::maths::clamp((int)ceil(currX) + 1, 0, input.cols - 1);
      prevY = utils::maths::clamp((int)floor(currY) - 1, 0, input.rows - 1);
      nextY = utils::maths::clamp((int)ceil(currY) + 1, 0, input.rows - 1);

      prevRow = ((nextX - currX) / (nextX - prevX)) * input.at<uint8_t>(prevY,
                                                                    prevX)
              + ((currX - prevX) / (nextX - prevX)) * input.at<uint8_t>(prevY, nextX);
      nextRow = ((nextX - currX) / (nextX - prevX)) * input.at<uint8_t>(nextY, prevX)
                + ((currX - prevX) / (nextX - prevX)) * input.at<uint8_t>(nextY, nextX);
      totalLerp = ((nextY - currY) / (nextY - prevY)) * prevRow
                  + ((currY - prevY) / (nextY - prevY)) * nextRow;

      output.at<uchar>(y, x) = (uint8_t)totalLerp;

    }
  }

}

}

}