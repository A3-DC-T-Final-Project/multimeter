#ifndef _VOLTAGE_H_
#define _VOLTAGE_H_

#include "mbed.h"

#define V_100M_Range 0
#define V_1_Range 1
#define V_5_Range 2
#define V_10_Range 3

#define VOLTAGE_DEBUG 0

class Voltage {
    private:
        DigitalOut * vRangeA0;
        DigitalOut * vRangeA1;
        AnalogIn * vIn;
        int range;
        float upperVoltage, lowerVoltage, upperBound, lowerBound, expectedLower, expectedUpper, rangeUpperBound, rangeLowerBound;
        Serial * voltageSerial;
        void changeVoltageRange(int range);
        void measureDC(char * voltage);
        void measureAC(char * voltage);
        void calculateExpectedVoltages();
    public:
        void initVoltage(Serial * serial);
        char * measureVoltage(int mode);
        float getVREF();
        float getVDDA();
};

#endif /* _VOLTAGE_H_ */