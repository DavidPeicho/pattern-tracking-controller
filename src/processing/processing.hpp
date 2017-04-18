#pragma once

#include <algorithm>

#include <math.h>

#include <opencv2/opencv.hpp>

#include <utils/utils.hpp>

namespace ptc {

namespace processing {

enum ThresholdType {
  VALUE,
  SAUVOLA,
  OTSU
};

uint8_t
computeOtsuThreshold(cv::Mat& output);

uint8_t
computeSauvolaThreshold(cv::Mat& output, size_t windowSize);

void
binarize(cv::Mat& output, cv::Mat& input, ThresholdType thresholdType);

void
grayscale(cv::Mat& output, cv::Mat& input);

void
downscaleBy(cv::Mat& output, cv::Mat& input, double factor);

}

}
