#pragma once

namespace ptc {

namespace utils {

namespace maths {

template <typename T>
const T
clamp(const T& val, const T& min, const T& max) {

  return (val < min) ? min : ((val > max) ? max : val);

}

}

}

}