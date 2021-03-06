#pragma once

#include <memory>

#include <input/video-reader.hpp>
#include <processing/processing.hpp>
#include <processing/convolution.hpp>

#include <data.hpp>
#include <utils/logger.hpp>
#include "input-processor.hpp"

namespace ptc {

enum Direction {

  UP,
  DOWN,
  LEFT,
  RIGHT

};

class TrackerImpl {

  public:
    TrackerImpl();
    ~TrackerImpl();

  public:
    void
    start();

    bool
    update(const std::shared_ptr<event::InputProcessor>& inputProcessor);

    void
    stop();

    void
    preprocessFrame(const cv::Mat& input);

    bool
    processFrame(const cv::Mat& input, const
                 std::shared_ptr<event::InputProcessor>& inputProcessor);

    void
    requestNextFrame();

  public:
    const cv::Mat&
    getRawFrame() const;

    const cv::Mat&
    getFrame(data::Frame frameType) const;

    const std::vector<cv::Point>&
    arrowShape() const;

    int
    getWidth() const;

    int
    getHeight() const;

    void
    setDebugFrame(std::shared_ptr<cv::Mat>& frame);

  private:
    void
    computeEvents(const std::shared_ptr<event::InputProcessor>& inputProcessor);

  private:
    void
    init(int h, int w);

  private:
    static int          MIN_SIZE;
    static unsigned int MAX_PTS_NB;
    static double       MIN_SCALE_FACTOR;

  private:
    VideoReader                           v_;

    int                                   h_;
    int                                   w_;

    double                                scaleFactor_;

    bool*                                 cachedVisited_;

    cv::Mat                               rawFrame_;
    std::shared_ptr<cv::Mat>              debugFrame_;

    std::vector<std::shared_ptr<cv::Mat>> frames_;

    std::vector<cv::Point>                arrowContour_;
    std::vector<cv::Point>                arrowPoints_;

};

}
