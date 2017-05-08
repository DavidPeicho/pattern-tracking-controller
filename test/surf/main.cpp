#include <iostream>
#include <ptc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <surf/surf.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <unistd.h>
#include <processing/processing.hpp>

int main() {
  std::cout << "Hello World!" << std::endl;
  auto tracker = ptc::Tracker::instance();
  cv::Mat frame;

  //const char* imgPath = "assets/cat.png";
  //const char* imgPath = "assets/text.png";
  //const char* imgPath = "assets/test.png";
  const char* imgPath = "assets/scene.png";

  frame = cv::imread(imgPath, CV_LOAD_IMAGE_COLOR);

  if(!frame.data) {
    std::cerr <<  "Could not open or find the image" << std::endl ;
    return -1;
  }

  // Initial image
  cv::imwrite("test_input.png", frame);

  // Integral image
  cv::Mat copy(frame.size(), frame.type());
  //processing::integralImage(copy, img);
  cv::integral(frame, copy);
  cv::imwrite("test_integral.png", copy);

  // Interest points
  ptc::surf::Surf surf(frame);
  surf.launch();
  cv::imwrite("test_output.png", frame);

  char temp[120];
  std::cout << "Results saved into " << (getcwd(temp, 120) ? std::string( temp ) : std::string("")) << std::endl;

  tracker->free();

  return 0;
}