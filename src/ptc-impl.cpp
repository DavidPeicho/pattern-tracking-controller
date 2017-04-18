#include <ptc-impl.hpp>

namespace ptc {

int TrackerImpl::MIN_SIZE = 200;

void
TrackerImpl::start() {

  v_.open();

}

void
TrackerImpl::update() {

  auto frame = v_.nextFrame();

}

void
TrackerImpl::stop() {

  v_.close();

}

std::shared_ptr<cv::Mat>
TrackerImpl::processFrame(cv::Mat& input) {

  utils::Logger::instance()->enable(true);
  utils::Logger::instance()->start("Begin frame processing...");

  int w = input.size().width;
  int h = input.size().height;

  // Grayscales the image.
  cv::Mat gray = cv::Mat::zeros(h, w, CV_8UC1);
  processing::grayscale(gray, input);

  cv::Mat smoothedGray = cv::Mat::zeros(h, w, CV_8UC1);
  processing::conv::applyConvolution(smoothedGray, gray, processing::conv::GAUSSIAN5_K);

  // Downscales the image.
  // It is important to note we do not downscale the image
  // if it already has a low resolution.
  double sFactor = std::min(w / (double)TrackerImpl::MIN_SIZE,
                            h / (double)TrackerImpl::MIN_SIZE);

  std::shared_ptr<cv::Mat> binarized = nullptr;
  if (sFactor < 2.0) {
    binarized = std::make_shared<cv::Mat>(h, w, CV_8UC1);
    processing::binarize(*binarized, smoothedGray, processing::ThresholdType::OTSU);
    utils::Logger::instance()->stop("End frame processing...");
    return binarized;
  }

  auto scaled = cv::Mat((int)(h / sFactor), (int)(w / sFactor), CV_8UC1);
  processing::downscaleBy(scaled, smoothedGray, sFactor);

  binarized = std::make_shared<cv::Mat>((int)(h / sFactor), (int)(w / sFactor), CV_8UC1);
  processing::binarize(*binarized, scaled, processing::ThresholdType::VALUE);

  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i>              hierarchy;

  findContours(*binarized, contours, hierarchy, CV_RETR_EXTERNAL,
               CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

  std::vector<std::vector<cv::Point> > contoursPoly(contours.size());
  for (size_t i = 0; i < contours.size(); ++i) {
    approxPolyDP(cv::Mat(contours[i]), contoursPoly[i], 3, true );
  }

  utils::Logger::instance()->stop("End frame processing...");

  return binarized;
}

}
