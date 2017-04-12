#include <utils/logger.hpp>

namespace ptc {

namespace utils {

Logger* Logger::instance_ = nullptr;

void
Logger::start(const std::string msg) {

  if (this->isActivated()) {
    this->startTime_ = std::chrono::system_clock::now();
    std::cout << msg << std::endl;
  }

}

void
Logger::stop(const std::string msg) {

  if (this->isActivated()) {
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - this->startTime_;
    std::cout << msg << " [" << elapsed.count() << "s] " << std::endl;
  }

}

}

}
