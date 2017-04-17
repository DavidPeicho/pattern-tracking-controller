#include <ptc-impl.hpp>

namespace ptc {

int           TrackerImpl::MIN_SIZE = 300;
unsigned int  TrackerImpl::MAX_PTS_NB = 12;
double        TrackerImpl::MIN_SCALE_FACTOR = 2.0;
double        TrackerImpl::MIN_EVT_DIST = 75.0;

TrackerImpl::TrackerImpl()
            : h_{-1}, w_{-1}
            , scaleFactor_(0.0)
            , cachedVisited_{nullptr} {

  arrowPoints_.reserve(3);
  arrowContour_.reserve(MAX_PTS_NB);

}

TrackerImpl::~TrackerImpl() {

  delete[] cachedVisited_;

}

void
TrackerImpl::start() {

  utils::Logger::instance()->enable(true);
  v_.open();

}

void
TrackerImpl::update() {

  rawFrame_ = v_.nextFrame();

}

void
TrackerImpl::stop() {

  v_.close();

}

void
TrackerImpl::preprocessFrame(const cv::Mat& input) {

  // TODO: Improve preprocessing by renforcing obtained contours.

  // Setups the memory once in order to reduce allocations
  if (h_ == -1 && w_ == -1) {
    this->init(input.size().height, input.size().width);
  }

  auto& gray = *frames_[data::Frame::GRAY];
  auto& blurred = *frames_[data::Frame::BLURRED];
  auto& scaled = *frames_[data::Frame::SCALED];
  auto& bin = *frames_[data::Frame::BINARIZED];

  // Transforms the RGB image to a grayscale image.
  processing::grayscale(gray, input);
  // Blurs it to remove artifacts
  cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0, 0);

  // Scales the image by the biggest factor found
  if (scaleFactor_ >= MIN_SCALE_FACTOR) {
    processing::downscaleBy(scaled, blurred, scaleFactor_);
  }

  // Applies adaptive binarization
  cv::adaptiveThreshold(scaled, bin, 255,
                        cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                        cv::THRESH_BINARY_INV, 5, 2);
  // Adds white border around the image
  for (int i = 0; i < scaled.rows; ++i) {
    bin.at<uint8_t>(i, 0) = 255;
    bin.at<uint8_t>(i, scaled.cols - 1) = 255;
  }
  for (int i = 0; i < scaled.cols; ++i) {
    bin.at<uint8_t>(0, i) = 255;
    bin.at<uint8_t>(scaled.rows - 1, i) = 255;
  }

  //cv::Canny(scaled, bin, 25, 150, 3);

}

bool
TrackerImpl::processFrame(const cv::Mat& input,
                          event::InputProcessor& inputProcessor) {

  arrowPoints_.clear();
  arrowContour_.clear();

  std::vector<std::vector<cv::Point>> contours;

  for (int i = 0; i < h_ * w_; ++i) cachedVisited_[i] = false;

  processing::detection::findContours(input, contours, cachedVisited_);

  std::vector<std::vector<cv::Point> > contoursPoly(contours.size());
  for (size_t i = 0; i < contours.size(); ++i) {
    processing::detection::approxPoly(contours[i], contoursPoly[i], 0,
                                      contours[i].size() - 1);
  }

  for (size_t i = 0; i < contoursPoly.size(); ++i) {

    if (contoursPoly[i].size() >= 5 && contoursPoly[i].size() <= MAX_PTS_NB) {

      if (processing::detection::isArrow(contoursPoly[i], arrowPoints_)) {

        // Saves contour in the container exposed by the API
        std::copy(contoursPoly[i].begin(),
                  contoursPoly[i].end(),
                  std::back_inserter(arrowContour_));

        computeEvents(inputProcessor);
        return true;

      }

    }

  }

  return false;

}

const cv::Mat&
TrackerImpl::getRawFrame() const {

  return rawFrame_;

}

const cv::Mat&
TrackerImpl::getFrame(data::Frame type) const {

  return *frames_[type];

}

const std::vector<cv::Point>&
TrackerImpl::arrowShape() const {

  return arrowContour_;

}

void
TrackerImpl::computeEvents(event::InputProcessor& inputProcessor) {

  // Checks if arrow matches a given shape
  if (inputProcessor.isRegistered(event::Event::AT)) {
    auto& shape = inputProcessor.getArrowShape();
    if (shape.size() == arrowPoints_.size()) {
      if (utils::maths::inRange(shape[1], arrowPoints_[0], MIN_EVT_DIST) &&
          utils::maths::inRange(shape[0], arrowPoints_[2], MIN_EVT_DIST) &&
          utils::maths::inRange(shape[2], arrowPoints_[1], MIN_EVT_DIST)) {

        inputProcessor.call(event::Event::AT);

      }
    }
  }

  // Computes arrow orientation
  auto center = cv::Point((arrowPoints_[1].x + arrowPoints_[2].x) / 2,
                          (arrowPoints_[1].y + arrowPoints_[2].y) / 2);

  cv::Point2d dir(arrowPoints_[0] - center);

  double norm = sqrt(dir.x * dir.x + dir.y * dir.y);
  dir.x = dir.x / norm;
  dir.y = - dir.y / norm;

  //double angle = (atan2(dir.y, dir.x) * 180.0) / M_PI;
  double angle = (acos(dir.x) * 180.0) / M_PI;
  if (dir.y < 0.0) angle = 360.0 - angle;

  static auto rightBounds = std::make_tuple(-60.0, 60.0);
  static auto upBounds = std::make_tuple(30.0, 150.0);
  static auto leftBounds = std::make_tuple(120.0, 240.0);
  static auto downBounds = std::make_tuple(210.0, 330.0);

  if (utils::maths::inBounds(angle, rightBounds)) {
    inputProcessor.call(event::Event::LEFT);
  }
  if (utils::maths::inBounds(angle, upBounds)) {
    inputProcessor.call(event::Event::UP);
  }
  if (utils::maths::inBounds(angle, leftBounds)) {
    inputProcessor.call(event::Event::RIGHT);
  }
  if (utils::maths::inBounds(angle, downBounds)) {
    inputProcessor.call(event::Event::DOWN);
  }

}

void
TrackerImpl::init(int h, int w) {

  h_ = h;
  w_ = w;
  cachedVisited_ = new bool[w_ * h_];

  // Allocates temporary frames only once.
  // This will reduce the overhead create by stack / heap allocations.
  frames_.reserve(4);
  frames_.push_back(std::make_shared<cv::Mat>(h_, w_, CV_8UC1));
  frames_.push_back(std::make_shared<cv::Mat>(h_, w_, CV_8UC1));

  // DEBUG
  cv::Mat graySmoothed = cv::Mat::zeros(h, w, CV_8UC1);
  processing::conv::applyConvolution(graySmoothed, gray,
                                     processing::conv::GAUSSIAN5_K);
  auto output = std::make_shared<cv::Mat>(h, w, CV_8UC1);
  auto sobelConvolution = [](double a, double b) {
    return sqrt(a * a + b * b);
  };
  processing::conv::applyBiconvolution(*output, graySmoothed,
                                       processing::conv::SOBELX_K,
                                       processing::conv::SOBELY_K,
                                       sobelConvolution);
  return output;
  // END DEBUG

  // Downscales the image.
  // It is important to note we do not downscale the image
  // if it already has a low resolution.
  scaleFactor_ = std::min(w_ / (double)TrackerImpl::MIN_SIZE,
                          h_ / (double)TrackerImpl::MIN_SIZE);
  if (scaleFactor_ >= MIN_SCALE_FACTOR) {
    h_ = (int)(h_ / scaleFactor_);
    w_ = (int)(w_ / scaleFactor_);
    frames_.push_back(std::make_shared<cv::Mat>(h_, w_, CV_8UC1));
  } else {
    frames_.push_back(frames_[data::Frame::BLURRED]);
  }

  frames_.push_back(std::make_shared<cv::Mat>(h_, w_, CV_8UC1));
}

}
