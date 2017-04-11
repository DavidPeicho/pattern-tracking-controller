#pragma once

#include <algorithm>

#include <math.h>

#include <opencv2/opencv.hpp>

#include <utils/utils.h>

namespace ptc {

namespace processing {

int
computeLocalThreshold(cv::Mat& output, size_t windowSize);

void
binarize(cv::Mat& output, cv::Mat& input);

void
grayscale(cv::Mat& output, cv::Mat& input);

void
downscaleBy(cv::Mat& output, cv::Mat& input, double factor);

}

}
