#ifndef _CURRENT_H_
#define _CURRENT_H_

#include "mbed.h"

#include "voltage.hpp"

// All in milli Amps
#define I_10 0
#define I_25 1
#define I_50 2
#define I_100 3

class Current {
    private:
        DigitalOut * iRangeA0;
        DigitalOut * iRangeA1;
        AnalogIn * iIn;
        Serial * currentSerial;
        Voltage * voltage_ptr;
        int range;
        float upperVoltage, lowerVoltage, upperBound, lowerBound, expectedLower, expectedUpper, rangeUpperBound, rangeLowerBound;
        void changeCurrentRange(int range);
        void calculateExpectedCurrents();
    public:
        void initCurrent(Serial * serial, Voltage * voltage);
        char * measureCurrent();
};

#endif // _CURRENT_H_