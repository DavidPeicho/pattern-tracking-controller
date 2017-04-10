#pragma once

#include <memory>
#include <opencv2/core/mat.hpp>

namespace ptc {

  class TrackerImpl;

}

namespace ptc {

class Tracker {

  public:
    typedef std::shared_ptr<Tracker> Ptr;

  public:
    Ptr
    static inline
    instance() {

      if (!instance_) {
        instance_ = std::unique_ptr<Tracker>(new Tracker);
      }
      return instance_;

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

    void
    processFrame(cv::Mat& output, cv::Mat& input);

  private:
    Tracker();
    Tracker(const Tracker&);
    Tracker& operator=(const Tracker&);

  private:
    static Ptr instance_;

  private:
    std::unique_ptr<TrackerImpl> pimpl_;

};

Tracker::Ptr Tracker::instance_ = nullptr;

}
