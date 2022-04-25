#ifndef _RESISTANCE_H_
#define _RESISTANCE_H_

#include "mbed.h"

#include "voltage.hpp"

#define R_10U 0
#define R_100U 1
#define R_1M 2

class Resistance {
    private:
        int range;
        DigitalOut * rRangeA0;
        DigitalOut * rRangeA1;
        Serial * resistanceSerial;
        Voltage * voltage_ptr;
        void changeCurrentRange(int range);

    public:
        void initResistance(Serial * serial, Voltage * voltage);
        char * measureResistance(bool * buttonIsPressed);
        uint8_t omega[8][5] = {
            {0, 0, 0, 0, 0},
            {0, 1, 1, 1, 0},
            {1, 0, 0, 0, 1},
            {1, 0, 0, 0, 1},
            {1, 0, 0, 0, 1},
            {0, 1, 0, 1, 0},
            {1, 1, 0, 1, 1},
            {0, 0, 0, 0, 0}
        };
};

#endif /* _RESISTANCE_H_ */
