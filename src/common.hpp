#ifndef _COMMON_H_
#define _COMMON_H_

namespace Common {
    float map(float x, float in_min, float in_max, float out_min, float out_max);
    float calculateBound(float vref, float expectedVoltage);
};

#endif /* _COMMON_H_ */