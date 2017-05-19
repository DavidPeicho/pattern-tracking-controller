#pragma once

#include <memory>
#include <opencv2/core/mat.hpp>

#include <data.hpp>
#include <input-processor.hpp>

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

    bool
    update();

    void
    stop();

  public:
    void
    setDebugFrame(std::shared_ptr<cv::Mat>& frame);

  public:
    void
    inputProcessor(event::InputProcessor i);

    const cv::Mat&
    getRawFrame() const;

    const cv::Mat&
    getFrame(data::Frame frameType) const;

    const std::vector<cv::Point>&
    arrowShape() const;

  private:
    Tracker();
    Tracker(const Tracker&);
    Tracker& operator=(const Tracker&);

  private:
    static Tracker* instance_;

  private:
    event::InputProcessor         inputProcessor_;

    std::shared_ptr<TrackerImpl>  pimpl_;

};
  
} //
