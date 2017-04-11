#include <ptc.hpp>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>

int main() {

  auto tracker = ptc::Tracker::instance();
  cv::Mat frame;
  frame = cv::imread("assets/cat.png", CV_LOAD_IMAGE_COLOR);

  if(!frame.data) {
    std::cerr <<  "Could not open or find the image" << std::endl ;
    return -1;
  }

  /*cv::Mat result = cv::Mat::zeros(frame.size().height, frame.size().width,
                                  CV_8UC1);*/

  cv::Mat result = cv::Mat::zeros(frame.size().height / 2, frame.size().width
                                                           / 2,
                                  CV_8UC1);
  tracker->processFrame(result, frame);

  cv::imwrite("result.png", result);

  return 0;

}
