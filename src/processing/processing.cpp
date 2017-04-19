#include <processing/processing.hpp>

namespace ptc {

namespace processing {

uint8_t
computeOtsuThreshold(cv::Mat& input) {

  int* histogram = new int[256];
  for (size_t i = 0; i < 256; ++i) histogram[i] = 0;

  // Builds histogram
  for (int y = 0; y < input.rows; ++y) {
    for (int x = 0; x < input.cols; ++x) {
      uint8_t val = input.at<uint8_t>(y, x);
      histogram[val] = histogram[val] + 1;
    }
  }

  int sum = 0;
  for (size_t i = 1; i < 256; ++i) sum += i * histogram[i];

  int totalNbPx = input.rows * input.cols;
  int sumB = 0;
  int wB = 0;
  int wF = 0;
  double mB = 0;
  double mF = 0;
  double max = 0.0;
  double between = 0.0;
  double t1 = 0.0;
  double t2 = 0.0;
  for (size_t i = 0; i < 256; ++i) {
    wB += histogram[i];
    if (wB == 0) continue;

    wF = totalNbPx - wB;
    if (wF == 0) break;

    sumB += i * histogram[i];
    mB = sumB / wB;
    mF = (sum - sumB) / wF;
    between = wB * wF * (mB - mF) * (mB - mF);
    if (between >= max) {
      t1 = i;
      if (between > max) t2 = i;
      max = between;
    }
  }

  delete[] histogram;

  return (uint8_t)((t1 + t2) / 2.0);

}

uint8_t
computeSauvolaThreshold(cv::Mat& input, int yInit, int xInit, int wSize) {

  // This function implements the Sovola algorithm.
  // TODO: One good optimization would be to use the Integral Images
  // TODO: that brings this algorithm closer to a O(1) algorithm.
  int count = 0;
  double mean = 0;
  double meanSquare = 0;
  double s = 0;

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

  return (uint8_t)(mean * (1 + 0.6 * ((s / 128.0) - 1)));

}

void
binarize(cv::Mat& output, cv::Mat& input, ThresholdType thresholdType) {

  uint8_t threshold = 10;
  if (thresholdType == ThresholdType::OTSU) {
    threshold = computeOtsuThreshold(input);
  }

  for (int y = 0; y < input.rows; ++y) {
    for (int x = 0; x < input.cols; ++x) {
      uint8_t& val = input.at<uchar>(y, x);
      if (thresholdType == ThresholdType::SAUVOLA) {
        threshold = computeSauvolaThreshold(input, y, x, 25);
      }
      if (val >= threshold) {
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

  uint8_t p1 = 0;
  uint8_t p2 = 0;
  uint8_t p3 = 0;
  uint8_t p4 = 0;

  double prevRow = 0.0;
  double nextRow = 0.0;
  double totalLerp = 0.0;

  for (int y = 0; y < output.rows; ++y) {
    for (int x = 0; x < output.cols; ++x) {
      double currX = (double)x * factor;
      double currY = (double)y * factor;

      // Gets derivatives x and y positions
      prevX = utils::maths::clamp((int)floor(currX - 1), 0, input.cols - 1);
      nextX = utils::maths::clamp((int)ceil(currX + 1), 0, input.cols - 1);
      prevY = utils::maths::clamp((int)floor(currY - 1), 0, input.rows - 1);
      nextY = utils::maths::clamp((int)ceil(currY + 1), 0, input.rows - 1);

      // Gets each 4 points value in the original image
      p1 = input.at<uint8_t>(prevY, prevX);
      p2 = input.at<uint8_t>(prevY, nextX);
      p3 = input.at<uint8_t>(nextY, prevX);
      p4 = input.at<uint8_t>(nextY, nextX);

      prevRow = ((nextX - currX) / (nextX - prevX)) * p1
              + ((currX - prevX) / (nextX - prevX)) * p2;
      nextRow = ((nextX - currX) / (nextX - prevX)) * p3
                + ((currX - prevX) / (nextX - prevX)) * p4;
      totalLerp = ((nextY - currY) / (nextY - prevY)) * prevRow
                  + ((currY - prevY) / (nextY - prevY)) * nextRow;

      output.at<uchar>(y, x) = (uint8_t)totalLerp;

    }
  }

}

void
integralImage(cv::Mat &output, cv::Mat& input) {
  grayscale(output, input);
  for (int y = 1; y < output.rows; ++y)
    output.at<uchar>(y, 0) = output.at<uchar>(0, 0);
  for (int x = 1; x < output.cols ; ++x)
    output.at<uchar>(0, x) = output.at<uchar>(0, 0);
  for (int y = 1; y < output.rows; ++y) {
    for (int x = 1; x < output.cols; ++x) {
      auto& px = output.at<cv::Vec3b>(y, x);
      uint8_t value = output.at<uchar>(y - 1, x) + output.at<uchar>(y, x - 1);
      output.at<uchar>(y, x) = value;
    }
  }
}

}

}