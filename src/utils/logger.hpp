#pragma once

#include <iostream>
#include <chrono>

namespace ptc {

namespace utils {

class Logger {

  public:
    static inline
    Logger*
    instance() {

      if (instance_ == nullptr) {
        instance_ = new Logger();
      }
      return instance_;

    }

  public:
    void
    start(const std::string msg = "");

    void
    stop(const std::string msg);

  public:
    inline void
    enable(bool status) {

      activated_ = status;

    }

    inline bool
    isActivated() {

      return activated_;

    }

  private:
    static Logger* instance_;

  private:
    bool                                                activated_;
    std::chrono::time_point<std::chrono::system_clock>  startTime_;

};

}

}
