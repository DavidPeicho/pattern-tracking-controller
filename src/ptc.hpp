#pragma once

#include <memory>
#include <opencv2/core/mat.hpp>

namespace ptc {

  class TrackerImpl;

}

namespace ptc {

class Tracker {

  public:
    static inline
    Tracker*
    instance() {

      if (instance_ == nullptr) {
        instance_ = new Tracker();
      }
      return instance_;

    }

    static inline
    void
    free() {
      if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
      }
    }

  public:
    ~Tracker();

  public:
    void
    start();

    void
    update();

    void
    stop();

    std::shared_ptr<cv::Mat>
    processFrame(cv::Mat& input);

  private:
    Tracker();
    Tracker(const Tracker&);
    Tracker& operator=(const Tracker&);

  private:
    static Tracker* instance_;

  private:
    std::shared_ptr<TrackerImpl> pimpl_;

};

Tracker* Tracker::instance_ = nullptr;

}
