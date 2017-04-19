#include <processing/processing.hpp>

namespace ptc {

  namespace processing {

    uint8_t
    computeOtsuThreshold(cv::Mat& input) {

      // TODO: Clearly, this function is buggy
      // TODO: it perfectly works using opencv Otsu thresholding

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
    integralImage(cv::Mat &output, cv::Mat &input) {
      grayscale(output, input);
      for (int y = 1; y < output.rows; ++y)
        output.at<uchar>(y, 0) += output.at<uchar>(y - 1, 0);
      for (int x = 1; x < output.cols; ++x)
        output.at<uchar>(0, x) += output.at<uchar>(0, x - 1);
      for (int y = 1; y < output.rows; ++y)
        for (int x = 1; x < output.cols; ++x)
          output.at<uchar>(y, x) += output.at<uchar>(y - 1, x) + output.at<uchar>(y, x - 1);
    }

    double
    getBoxIntegral(cv::Mat &data, int row, int col, int rows, int cols) {
      float A = 0, B = 0, C = 0, D = 0;
      int r1 = std::min(row, data.size().height - 1);
      int c1 = std::min(row, data.size().width - 1);
      int r2 = std::min(row + rows, data.size().height - 1);
      int c2 = std::min(col + cols, data.size().width - 1);
      if (r1 >= 0 && c1 >= 0) A = data.at<uchar>(r1, c1);
      if (r1 >= 0 && c2 >= 0) B = data.at<uchar>(r1, c2);
      if (r2 >= 0 && c1 >= 0) C = data.at<uchar>(r2, c1);
      if (r2 >= 0 && c2 >= 0) D = data.at<uchar>(r2, c2);
      return std::max(0.f, A - B - C + D);
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
    grayscale(cv::Mat& output, const cv::Mat& input) {

      for (int y = 0; y < input.rows; ++y) {
        for (int x = 0; x < input.cols; ++x) {
          auto& px = input.at<cv::Vec3b>(y, x);
          uint8_t value = (uint8_t)(0.299 * px[2] + 0.587 * px[1] + 0.114 * px[0]);
          output.at<uchar>(y, x) = value;
        }
      }

    }

    void
    downscaleBy(cv::Mat& output, const cv::Mat& input, double factor) {

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

    namespace detection {

      bool
      isArrow(std::vector<cv::Point>& points,
              std::vector<cv::Point>& arrowHeadPoints) {

        static auto pointerBounds = std::make_tuple(45.0, 59.9);
        static auto acuteBounds = std::make_tuple(60.0, 79.9);
        static auto rightBounds = std::make_tuple(80.0, 109.9);
        static auto largeBounds = std::make_tuple(110.0, 200.0);

        std::vector<double> angles;
        std::vector<size_t> pointerIdx;
        std::vector<size_t> acuteIdx;
        std::vector<size_t> largeIdx;
        std::vector<size_t> rightIdx;

        // Computes angle associated to every point
        size_t prevIdx = points.size() - 1;
        size_t nextIdx = 1;
        for (size_t i = 0; i < points.size(); ++i) {

          if (i == points.size() - 1) {
            nextIdx = 0;
          }

          auto prev = points[prevIdx];
          auto curr = points[i];
          auto next = points[nextIdx];

          double angle = utils::maths::getAngle(curr, prev, next);

          angles.push_back(angle);
          if (utils::maths::inBounds(angle, pointerBounds)) {
            pointerIdx.push_back(i);
          } else if (utils::maths::inBounds(angle, acuteBounds)) {
            acuteIdx.push_back(i);
          } else if (utils::maths::inBounds(angle, rightBounds)) {
            rightIdx.push_back(i);
          } else if (angle >= 110.0) {
            largeIdx.push_back(i);
          }

          prevIdx = i;
          ++nextIdx;

        }

        // Checks simple case that are impossible for an arrow
        if (rightIdx.size() < 2 || acuteIdx.size() == 0 || acuteIdx.size() >= 4) {
          return false;
        }

        // Checks topology whenever every angle matches the shape of an arrow.
        // The following loop can handle cases where the arrow is
        // completely visible, and also when the arrow is half cut on
        // the bottom, left, and right side.
        for (size_t i = 0; i < pointerIdx.size(); ++i) {
          size_t id = pointerIdx[i];
          size_t prevPtr = (id != 0) ? id - 1 : points.size() - 1;
          size_t nextPtr = (id != points.size() - 1) ? id + 1 : 0;

          // Checks simple cases whenever the head of the arrow
          // is completely visible.
          if (utils::maths::checkArrowBase(angles, prevPtr, nextPtr,
                                           acuteBounds,
                                           rightBounds)) {
            arrowHeadPoints.push_back(points[id]);
            arrowHeadPoints.push_back(points[prevPtr]);
            arrowHeadPoints.push_back(points[nextPtr]);
            return true;
          }

          // Checks case when arrow head is half out of the screen.
          size_t acute = 0;
          size_t largeNext = 0;
          size_t largeNextNext = 0;
          size_t acuteNext = 0;
          if (utils::maths::inBounds(angles[prevPtr], largeBounds) ||
              utils::maths::inBounds(angles[prevPtr], rightBounds)) {

            acute = nextPtr;
            largeNext = (prevPtr != 0) ? prevPtr - 1 : points.size() - 1;
            largeNextNext = (largeNext != 0) ? largeNext - 1 : points.size() - 1;
            acuteNext = (acute != points.size() - 1) ? acute + 1 : 0;

          } else if (utils::maths::inBounds(angles[nextPtr], largeBounds) ||
                     utils::maths::inBounds(angles[nextPtr], rightBounds)) {

            acute = prevPtr;
            acuteNext = (acute != 0) ? acute - 1 : points.size() - 1;
            largeNext = (nextPtr != points.size() - 1) ? nextPtr + 1 : 0;
            largeNextNext = (largeNext != points.size() - 1) ? largeNext + 1 : 0;


          } else {
            continue;
          }

          if (!utils::maths::inBounds(angles[acute], acuteBounds) ||
              (!utils::maths::inBounds(angles[largeNext], largeBounds) &&
               !utils::maths::inBounds(angles[largeNext], rightBounds))) {
            continue;
          }

          if (!utils::maths::inBounds(angles[largeNextNext], rightBounds) &&
              !utils::maths::inBounds(angles[acuteNext], acuteBounds)) {
            continue;
          }

          arrowHeadPoints.push_back(points[id]);
          arrowHeadPoints.push_back(points[acute]);
          arrowHeadPoints.push_back(points[largeNext]);
          return true;

        }

        // TODO: Finds a better way to handle the following case.
        // TODO: When we checks for two large angles, it can match
        // TODO: primitives that have really different shapes.

        // Checks topology whenever every angle matches the shape of an arrow.
        // Checks the case where the arrow pointer is out of the screen.
        if (largeIdx.size() != 2) return false;

        size_t prev = 0;
        size_t next = 0;
        size_t aId = largeIdx[0];
        size_t bId = largeIdx[1];
        if (bId > aId) {
          prev = (aId != 0) ? aId - 1 : points.size() - 1;
          next = (bId != points.size() - 1) ? bId + 1 : 0;
        } else {
          prev = (bId != 0) ? bId - 1 : points.size() - 1;
          next = (aId != points.size() - 1) ? aId + 1 : 0;
        }

        if (utils::maths::checkArrowBase(angles, prev, next, acuteBounds,
                                         rightBounds)) {
          cv::Point middle((points[aId].x + points[bId].x) / 2,
                           (points[aId].y + points[bId].y) / 2);
          arrowHeadPoints.push_back(middle);
          arrowHeadPoints.push_back(points[prev]);
          arrowHeadPoints.push_back(points[next]);

          return true;
        }

        return false;

      }

      void
      findContours(const cv::Mat& input,
                   std::vector<std::vector<cv::Point>>& contours, bool* visited) {

        int h = input.size().height;
        int w = input.size().width;

        static int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
        static int dy[] = {0, -1, -1, -1, 0, 1, 1, 1};

        int dir = 7;
        uint8_t px = 0;

        cv::Point start(0, 0);
        cv::Point prev(0, 0);
        cv::Point curr(0, 0);

        for (int y = 1; y < input.rows - 1; ++y) {
          for (int x = 1; x < input.cols - 1; ++x) {
            px = input.at<uchar>(y, x);

            if (px < 250) continue;

            if (visited[y * w + x]) continue;

            start.x = x;
            start.y = y;
            prev.x = -1;
            prev.y = -1;
            curr.x = x;
            curr.y = y;

            contours.push_back(std::vector<cv::Point>());
            auto& vec = contours[contours.size() - 1];
            vec.push_back(start);
            int c = 0;

            do {

              int d = 0;
              if (dir % 2 == 0) {
                d = (dir + 7) % 8;
              } else {
                d = (dir + 6) % 8;
              }

              int i = d;
              bool found = false;
              bool noSol = true;
              do {
                uint8_t nextPx = 0;
                if (utils::maths::inBounds(curr.x + dx[i], curr.y + dy[i], h, w)) {
                  nextPx = input.at<uchar>(curr.y + dy[i], curr.x + dx[i]);
                }
                if (nextPx >= 250) {
                  prev.x = curr.x;
                  prev.y = curr.y;
                  curr.x = curr.x + dx[i];
                  curr.y = curr.y + dy[i];

                  if (curr.x == start.x && curr.y == start.y) {
                    found = true;
                    break;
                  }
                  if (!visited[curr.y * w + curr.x]) {
                    vec.push_back(curr);
                    visited[curr.y * w + curr.x] =  true;
                    dir = i;
                    noSol = false;
                    break;
                  }

                }

                ++c;
                ++i;
                if (i == 8) i = 0;
              } while (i != d);

              if (found || noSol) break;

            } while (c < 2 || (curr != start));

          }

        }

      }

      void
      approxPoly(std::vector<cv::Point>& contour,
                 std::vector<cv::Point>& result,
                 int minB, int maxB) {

        static double epsilon = 5.0;

        std::vector<bool> seen((unsigned int)(maxB - minB) + 1);
        for (size_t i = 0; i < seen.size(); ++i) seen[i] = true;

        std::stack<std::pair<int, int>> stack;
        stack.push(std::make_pair(minB, maxB));

        double d = 0.0;
        double dmax = 0.0;
        int index = 0;

        while (!stack.empty()) {

          minB = stack.top().first;
          maxB = stack.top().second;
          stack.pop();

          d = 0.0;
          dmax = 0.0;
          index = minB;

          for (int i = minB + 1; i < maxB - 1; ++i) {
            if (seen[i]) {
              d = utils::maths::distanceToLine(contour[minB], contour[maxB],
                                               contour[i]);
              if (d > dmax) {
                index = i;
                dmax = d;
              }
            }
          }

          if (dmax > epsilon) {
            stack.push(std::make_pair(minB, index));
            stack.push(std::make_pair(index, maxB));
          } else {
            for (int i = minB + 1; i < maxB; ++i) seen[i] = false;
          }

        }

        std::vector<cv::Point> tmp;

        for (size_t i = 0; i < seen.size(); ++i) {
          if (seen[i]) tmp.push_back(contour[i]);
        }

        if (tmp.size() < 2) return;

        auto pFirst = tmp[0];
        auto pEnd = tmp[tmp.size() - 1];
        int error = abs(pFirst.x - pEnd.x) + abs(pFirst.y - pEnd.y);
        if (error < 10) {
          tmp.pop_back();
        }

        result.push_back(tmp[0]);
        for (size_t i = 1; i < tmp.size(); ++i) {
          error = abs(tmp[i].x - tmp[i + 1].x) + abs(tmp[i].y - tmp[i + 1].y);
          if (error >= 10) result.push_back(tmp[i]);
        }

      }

    }

  }

}