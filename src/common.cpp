#include "common.hpp"

namespace Common {
    // From https://www.arduino.cc/reference/en/language/functions/math/map/
    float map(float x, float in_min, float in_max, float out_min, float out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    float calculateBound(float vref, float expectedVoltage) {
        float bound = 0;

        // VREFINT has a typical value of 1.21V
        // Page 139 https://www.st.com/resource/en/datasheet/dm00037051.pdf
        bound = (vref * expectedVoltage) / 1.21;

        return bound;
    }
}