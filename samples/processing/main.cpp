#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>

#include <ptc.hpp>
#include <unistd.h>
#include <surf/hessian.hpp>

int main() {

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

  std::vector<InterestPoint> featurePoints;
  ptc::surf::Hessian::hessian(frame, 4, 4, 2, featurePoints);
  for (uint i = 0; i < featurePoints.size(); i++)
    std::cout << featurePoints[i].getX() << " : " << featurePoints[i].getY() << std::endl;

  auto outputPtr = tracker->processFrame(frame);
  cv::imwrite("result.png", *outputPtr);

  tracker->free();

  return 0;

}
