#include <iostream>
#include <ptc.hpp>
#include <surf/surf.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <unistd.h>
#include <math.h>
#include <processing/processing.hpp>
#include "opencv2/xfeatures2d.hpp"

bool surfProcessImage(const cv::Mat &testFrame, const cv::Mat &arrowUpFrame, cv::Mat &img_matches);

int main() {
  auto tracker = ptc::Tracker::instance();
  cv::Mat arrowUpInput;
  const char* arrowUpImgPath = "assets/bhl.jpg";

  arrowUpInput= cv::imread(arrowUpImgPath, CV_LOAD_IMAGE_GRAYSCALE);
  if(!arrowUpInput.data) {
    std::cerr <<  "Could not open or find the object image" << std::endl ;
    return -1;
  }

  cv::Mat img_matches;

  tracker->start();

  while (true) {

    tracker->update();
    auto& tmp = tracker->getRawFrame();
    tracker->preprocessFrame(tmp);
    auto& bin = tracker->getFrame(ptc::data::Frame::GRAY);

    if(!cv::waitKey(20)) break;

    if (!surfProcessImage(bin, arrowUpInput, img_matches)) {
      cv::imshow("frame", tmp);
      continue;
    }

    cv::imshow("frame", img_matches);
  }

  tracker->free();

  return 0;
}

bool surfProcessImage(const cv::Mat &testFrame, const cv::Mat &arrowUpFrame, cv::Mat &img_matches) {

  int minHessian(500);

  // Keypoints
  cv::Ptr<cv::xfeatures2d::SurfFeatureDetector> detector = cv::xfeatures2d::SurfFeatureDetector::create(minHessian);
  std::vector<cv::KeyPoint> keypointsTest, keypointsArrowUp;
  detector->detect(testFrame, keypointsTest);
  detector->detect(arrowUpFrame, keypointsArrowUp);

  // Feature vectors
  cv::Ptr<cv::xfeatures2d::SurfDescriptorExtractor> extractor = cv::xfeatures2d::SurfDescriptorExtractor::create();
  cv::Mat descriptorTest, descriptorArrowUp;
  extractor->compute(arrowUpFrame, keypointsArrowUp, descriptorArrowUp);
  extractor->compute(testFrame, keypointsTest, descriptorTest);

  /*
  cv::BFMatcher matcher;
  std::vector<cv::DMatch> matches;
  matcher.match(descriptorArrowUp, descriptorTest, matches);
  */
  cv::FlannBasedMatcher matcher;
  std::vector<cv::DMatch> matches;
  matcher.match(descriptorArrowUp, descriptorTest, matches);

  double max_dist = 0, min_dist = 100, dist = 0;
  //-- Quick calculation of max and min distances between keypoints
  for( int i = 0; i < descriptorArrowUp.rows; i++ ) {
    dist = matches[i].distance;
    if (dist < min_dist)
      min_dist = dist;
    else
      max_dist = dist;
  }

  //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
  std::vector<cv::DMatch> good_matches;

  for (int i = 0; i < descriptorArrowUp.rows; i++)
    if (matches[i].distance < 3 * min_dist)
      good_matches.push_back( matches[i]);


  img_matches.data = NULL;


  cv::drawMatches(arrowUpFrame, keypointsArrowUp, testFrame, keypointsTest,
                  good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
                  std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

  std::vector<cv::Point2f> obj;
  std::vector<cv::Point2f> scene;
  for (unsigned int i = 0; i < good_matches.size(); i++) {
    obj.push_back(keypointsArrowUp[good_matches[i].queryIdx].pt);
    scene.push_back(keypointsTest[good_matches[i].trainIdx].pt);
  }
  cv::Mat H = findHomography(obj, scene, CV_RANSAC);

  // Get the corners from the image_1 ( the object to be "detected" )
  std::vector<cv::Point2f> obj_corners(4);
  obj_corners[0] = cvPoint(0,0);
  obj_corners[1] = cvPoint(arrowUpFrame.cols, 0);
  obj_corners[2] = cvPoint(arrowUpFrame.cols, arrowUpFrame.rows);
  obj_corners[3] = cvPoint(0, arrowUpFrame.rows);
  std::vector<cv::Point2f> scene_corners(4);

  cv::perspectiveTransform(obj_corners, scene_corners, H);

  float good_matches_ratio = (float)good_matches.size() / (float)matches.size();

  std::cout << good_matches_ratio << " ";
  if (good_matches_ratio > 0.25) {
    // Get the rotation of the image
    double a = H.at<double>(0, 0), b = H.at<double>(0, 1);
    float r = cv::fastAtan2(b, a);
    if (r > 315 || r <= 45)
      std::cout << "UP" << std::endl;
    if (r > 45 && r <= 135)
      std::cout << "RIGHT" << std::endl;
    if (r > 135 && r <= 225)
      std::cout << "DOWN" << std::endl;
    if (r > 225 && r <= 315)
      std::cout << "LEFT" << std::endl;
  }
  else
    std::cout << std::endl;

  //-- Draw lines between the corners (the mapped object in the scene - image_2 )
  line(img_matches, scene_corners[0] + cv::Point2f(arrowUpFrame.cols, 0),
       scene_corners[1] + cv::Point2f(arrowUpFrame.cols, 0), cv::Scalar(0, 255, 0), 4);
  line(img_matches, scene_corners[1] + cv::Point2f(arrowUpFrame.cols, 0),
       scene_corners[2] + cv::Point2f(arrowUpFrame.cols, 0), cv::Scalar( 0, 255, 0), 4);
  line(img_matches, scene_corners[2] + cv::Point2f(arrowUpFrame.cols, 0),
       scene_corners[3] + cv::Point2f(arrowUpFrame.cols, 0), cv::Scalar( 0, 255, 0), 4);
  line(img_matches, scene_corners[3] + cv::Point2f(arrowUpFrame.cols, 0),
       scene_corners[0] + cv::Point2f(arrowUpFrame.cols, 0), cv::Scalar( 0, 255, 0), 4);

  //-- Show detected matches
  return img_matches.data != NULL;
}
