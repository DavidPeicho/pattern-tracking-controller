#include <ptc.hpp>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>

int main() {

  auto tracker = ptc::Tracker::instance();
  cv::Mat frame;

  const char* imgPath = "assets/cat.png";
  //const char* imgPath = "assets/text.png";
  //const char* imgPath = "assets/test.png";

  frame = cv::imread(imgPath, CV_LOAD_IMAGE_COLOR);

  if(!frame.data) {
    std::cerr <<  "Could not open or find the image" << std::endl ;
    return -1;
  }

  auto outputPtr = tracker->processFrame(frame);
  cv::imwrite("result.png", *outputPtr);

  tracker->free();

  return 0;

}
