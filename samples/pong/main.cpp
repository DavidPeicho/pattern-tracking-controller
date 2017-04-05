#include <ptc.hpp>

int main() {

  auto tracker = ptc::Tracker::instance();
  tracker->start();

  /*while (true) {

    tracker->update();

  }*/

  tracker->stop();

  return 0;

}
