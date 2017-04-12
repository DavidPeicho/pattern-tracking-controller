#pragma once

#include <algorithm>
#include <stack>

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
grayscale(cv::Mat& output, const cv::Mat& input);

void
downscaleBy(cv::Mat& output, const cv::Mat& input, double factor);

namespace detection {

bool
isArrow(std::vector<cv::Point>& points,
        std::vector<cv::Point>& arrowHeadPoints);

void
findContours(const cv::Mat& input,
             std::vector<std::vector<cv::Point>>& contours, bool* visited);


void
approxPoly(std::vector<cv::Point>& contour,
           std::vector<cv::Point>& result,
           int minB, int maxB);

}

}

}
