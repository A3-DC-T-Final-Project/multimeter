#ifndef _VOLTAGE_H_
#define _VOLTAGE_H_

#include "mbed.h"

#define V_100M_Range 0
#define V_1_Range 1
#define V_5_Range 2
#define V_10_Range 3

class Voltage {
    private:
        DigitalOut * vRangeA0;
        DigitalOut * vRangeA1;
        AnalogIn * vIn;
        int range;
        void changeVoltageRange(int range);
        void measureDC(char * voltage);
        void measureAC(char * voltage);
    public:
        void initVoltage();
        char * measureVoltage(int mode);
        void pinsTest();
        void displayVREF();
};

#endif /* _VOLTAGE_H_ */