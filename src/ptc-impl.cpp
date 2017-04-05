#include <ptc-impl.hpp>

namespace ptc {

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

}
