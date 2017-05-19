#include <iostream>
#include <ptc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <surf/surf.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <unistd.h>
#include <processing/processing.hpp>
#include "opencv2/xfeatures2d.hpp"


int main() {
  auto tracker = ptc::Tracker::instance();
  cv::Mat testFrame;
  cv::Mat arrowUpFrame;

  //const char* testImgPath = "assets/cat.png";
  //const char* testImgPath = "assets/text.png";
  //const char* testImgPath = "assets/test.png";
  const char* testImgPath = "assets/scene.png";
  const char* arrowUpImgPath = "assets/arrow_up.jpg";

  testFrame = cv::imread(testImgPath, CV_LOAD_IMAGE_COLOR);
  arrowUpFrame = cv::imread(arrowUpImgPath, CV_LOAD_IMAGE_COLOR);

  if(!testFrame.data) {
    std::cerr <<  "Could not open or find the test image" << std::endl ;
    return -1;
  }
  if(!arrowUpFrame.data) {
    std::cerr <<  "Could not open or find the object image" << std::endl ;
    return -1;
  }

  // Initial image
  cv::imwrite("test_input.png", testFrame);

  int minHessian(400);

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

  // Matching using FLANN
  cv::FlannBasedMatcher matcher;
  std::vector<cv::DMatch> matches;
  matcher.match(descriptorArrowUp, descriptorTest, matches);

  //-- Quick calculation of max and min distances between keypoints
  double max_dist = 0; double min_dist = 100;
  double dist = 0;
  for( int i = 0; i < descriptorArrowUp.rows; i++ ) {
    dist = matches[i].distance; // distance between the two descriptors that match
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }

  // Keep only the matches within 3 * min_dist
  std::vector<cv::DMatch> good_matches;
  for (int i = 0; i < descriptorArrowUp.rows; i++)
    if (matches[i].distance < 3 * min_dist)
      good_matches.push_back(matches[i]);
  cv::Mat img_matches;
  drawMatches(arrowUpFrame, keypointsArrowUp, testFrame, keypointsTest,
               good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
               std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

  // Localize the object
  std::vector<cv::Point2f> obj;
  std::vector<cv::Point2f> scene;

  for(unsigned int i = 0; i < good_matches.size(); i++ ) {
    // Get the keypoints from the good matches
    obj.push_back(keypointsArrowUp[good_matches[i].queryIdx].pt);
    scene.push_back(keypointsTest[good_matches[i].trainIdx].pt);
  }

  cv::Mat H = findHomography(obj, scene, CV_RANSAC);
  //-- Get the corners from the image_1 ( the object to be "detected" )
  std::vector<cv::Point2f> obj_corners(4);
  obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint(arrowUpFrame.cols, 0);
  obj_corners[2] = cvPoint(arrowUpFrame.cols, arrowUpFrame.rows); obj_corners[3] = cvPoint(0, arrowUpFrame.rows);
  std::vector<cv::Point2f> scene_corners(4);

  perspectiveTransform( obj_corners, scene_corners, H);

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
  if (img_matches.data == NULL)
    std::cerr << "Data of img_matches is null" << std::endl;
  cv::namedWindow("Good Matches & Object detection", cv::WINDOW_AUTOSIZE); // Create a window for display.
  imshow("Good Matches & Object detection", img_matches);
  cv::imwrite("test_output_cv.png", img_matches);
  cv::waitKey(0);


  //cv::imwrite("test_output_own.png", testFrame);

  // Interest points
  /*
  ptc::surf::Surf surf(testFrame);
  surf.launch();
  cv::imwrite("test_output_own.png", testFrame);

  char temp[120];
  std::cout << "Results saved into " << (getcwd(temp, 120) ? std::string( temp ) : std::string("")) << std::endl;
  */

  tracker->free();

  return 0;
}