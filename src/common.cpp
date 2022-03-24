#include "common.hpp"

namespace Common {
    // From https://www.arduino.cc/reference/en/language/functions/math/map/
    float map(float x, float in_min, float in_max, float out_min, float out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
}