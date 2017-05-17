#include <iostream>
#include <opencv2/imgcodecs.hpp>

#include <ptc.hpp>
#include <opencv/cv.hpp>

int main() {

  auto tracker = ptc::Tracker::instance();

  ptc::event::InputProcessor processor;
  processor.registerEvent(ptc::event::Event::DOWN, []() -> void {

    std::cout << "DOWN" << std::endl;

  });

  tracker->inputProcessor(processor);
  tracker->start();

  while (true) {

    auto found = tracker->update();

    if(cv::waitKey(20) >= 0) break;

    auto& tmp = tracker->getRawFrame();
    if (!found) {
      cv::imshow("frame", tmp);
      continue;
    }

    auto& gray = tracker->getFrame(ptc::data::Frame::GRAY);
    auto& blur = tracker->getFrame(ptc::data::Frame::BLURRED);
    auto& bin = tracker->getFrame(ptc::data::Frame::BINARIZED);

    int h = tmp.size().height;
    int w = tmp.size().width;
    std::vector<std::vector<cv::Point>> c;
    c.push_back(tracker->arrowShape());
    cv::Mat result = cv::Mat::zeros(h, w, CV_8UC3);
    tmp.copyTo(result);
    cv::Scalar color = cv::Scalar(255, 0, 0);
    drawContours(result, c, 0, color, 5, 8, std::vector<cv::Vec4i>(), 0,
                 cv::Point());
    cv::imshow("frame", result);

  }

  tracker->free();

  return 0;

}
