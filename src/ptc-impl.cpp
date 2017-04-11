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

  int w = input.size().width;
  int h = input.size().height;

  // Grayscales the image.
  cv::Mat gray = cv::Mat::zeros(h, w, CV_8UC1);
  processing::grayscale(gray, input);

  // Downscales the image.
  // It is important to note we do not downscale the image
  // if it already has a low resolution.
  double sFactor = std::min(w / TrackerImpl::MIN_SIZE,
                            h / TrackerImpl::MIN_SIZE);

  std::shared_ptr<cv::Mat> output = nullptr;
  if (sFactor < 2.0) {
    output = std::make_shared<cv::Mat>(h, w, CV_8UC1);
    processing::binarize(*output, gray, processing::ThresholdType::OTSU);
    return output;
  }

  output = std::make_shared<cv::Mat>((int)(h / sFactor), (int)(w / sFactor), CV_8UC1);
  auto scaled = cv::Mat((int)(h / sFactor), (int)(w / sFactor), CV_8UC1);

  processing::downscaleBy(scaled, gray, sFactor);
  processing::binarize(*output, scaled, processing::ThresholdType::OTSU);

  return output;
}

}
