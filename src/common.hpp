#ifndef _COMMON_H_
#define _COMMON_H_

#include "mbed.h"

namespace CommonUtils {
    float map(float x, float in_min, float in_max, float out_min, float out_max);
    float calculateBound(float vref, float expectedVoltage);
};

class Common {
    private:
        AnalogIn * input;
    public:
        AnalogIn * initInput();
};

#endif /* _COMMON_H_ */