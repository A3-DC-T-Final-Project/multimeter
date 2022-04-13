#ifndef _VOLTAGE_H_
#define _VOLTAGE_H_

#include "mbed.h"
#include "opamps_conf.hpp"

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
        float upperVoltage, lowerVoltage;
        Serial * voltageSerial;
        void changeVoltageRange(int range);
        void measureDC(char * voltage, OpAmpsConf * opAmpsConf);
        void measureAC(char * voltage, OpAmpsConf * opAmpsConf);
    public:
        void initVoltage(Serial * serial, AnalogIn * input);
        char * measureVoltage(int mode);
        float getVREF();
        float getVDDA();
};

#endif /* _VOLTAGE_H_ */