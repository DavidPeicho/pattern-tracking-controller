#include <ptc-impl.hpp>

namespace ptc {

int           TrackerImpl::MIN_SIZE = 300;
unsigned int  TrackerImpl::MAX_PTS_NB = 12;
double        TrackerImpl::MIN_SCALE_FACTOR = 2.0;

TrackerImpl::TrackerImpl()
            : h_{-1}, w_{-1}
            , scaleFactor_(0.0)
            , cachedVisited_{nullptr}
            , debugFrame_{nullptr} {

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

  // Setups the memory once in order to reduce allocations
  if (h_ == -1 && w_ == -1) {
    this->init(v_.getHeight(), v_.getWidth());
  }

}

bool
TrackerImpl::update(const std::shared_ptr<event::InputProcessor>&
                    inputProcessor) {

  requestNextFrame();
  if (debugFrame_ == nullptr) {
    preprocessFrame(rawFrame_);
    auto bin = frames_[ptc::data::Frame::BINARIZED];
    return processFrame(*bin, inputProcessor);
  }

  preprocessFrame(*debugFrame_);
  auto bin = frames_[ptc::data::Frame::BINARIZED];
  return processFrame(*bin, inputProcessor);

}

void
TrackerImpl::stop() {

  v_.close();

}

void
TrackerImpl::preprocessFrame(const cv::Mat& input) {

  // TODO: Improve preprocessing by renforcing obtained contours.
  // TODO: Use erode and distort.
  // TODO:

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

}

bool
TrackerImpl::processFrame(const cv::Mat& input,
                          const std::shared_ptr<event::InputProcessor>&
                          inputProcessor) {

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

void
TrackerImpl::requestNextFrame() {

  rawFrame_ = v_.nextFrame();

}

int
TrackerImpl::getWidth() const {

  if (w_ != - 1) return w_;

  return v_.getWidth();

}

int
TrackerImpl::getHeight() const {

  if (h_ != -1) return h_;

  return v_.getHeight();

}

void
TrackerImpl::setDebugFrame(std::shared_ptr<cv::Mat>& frame) {

  debugFrame_ = frame;

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
TrackerImpl::computeEvents(const std::shared_ptr<event::InputProcessor>&
                           inputProcessor) {

  // Checks if arrow matches a given shape
  if (inputProcessor->isRegistered(event::Event::AT)) {
    auto& shape = inputProcessor->getTemplate();
    if (shape.size() == arrowPoints_.size()) {
      double minDist = inputProcessor->getMinMatchDistance();
      if (utils::maths::inRange(shape[1], arrowPoints_[0], minDist) &&
          utils::maths::inRange(shape[0], arrowPoints_[2], minDist) &&
          utils::maths::inRange(shape[2], arrowPoints_[1], minDist)) {

        inputProcessor->call(event::Event::AT);

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

  double angle = (acos(dir.x) * 180.0) / M_PI;
  if (dir.y < 0.0) angle = 360.0 - angle;

  bool noAction = true;
  if ((angle >= 308.0f && angle <= 360.0f) ||
      (angle >= 0.0f && angle <= 50.0f)) {
    inputProcessor->call(event::Event::LEFT);
    noAction = false;
  } else if (angle >= 120.0f && angle <= 230.0) {
    inputProcessor->call(event::Event::RIGHT);
    noAction = false;
  }

  if (angle >= 30.0f && angle < 150.0f) {
    inputProcessor->call(event::Event::UP);
    noAction = false;
  } else if (angle >= 215.0f && angle <= 330.0f) {
    inputProcessor->call(event::Event::DOWN);
    noAction = false;
  }

  inputProcessor->setArrowShape(arrowPoints_);
  inputProcessor->setAngle(angle);

  if (noAction) inputProcessor->call(event::Event::NONE);

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
