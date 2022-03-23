#include "mbed.h"

#include "modes.hpp"
#include "voltage.hpp"

extern "C" {
    #include <PB_LCD_Drivers.h>
}

void Voltage::initVoltage() {
    vRangeA0 = new DigitalOut(PE_3);
    vRangeA1 = new DigitalOut(PE_4);
    vIn = new AnalogIn(PC_4);

    range = V_10_Range;
    changeVoltageRange(range);
}

void Voltage::changeVoltageRange(int range) {
    switch (range) {
        case V_100M_Range:
            vRangeA0->write(0);
            vRangeA1->write(0);
            break;
        case V_1_Range:
            vRangeA0->write(1);
            vRangeA1->write(0);
            break;
        case V_5_Range:
            vRangeA0->write(0);
            vRangeA1->write(1);
            break;
        default:
        case V_10_Range:
            vRangeA0->write(1);
            vRangeA1->write(1);
            break;
    }
}

void Voltage::measureDC(char * voltage) {
    int i;
    float total = 0;
    for(i = 0; i < 1000; i++) {
        //total += vIn
    }
}

char * Voltage::measureVoltage(int mode) {
    char * voltage = (char *) malloc(12 * sizeof(char));
    snprintf(voltage, 0xC, "Placeholder");

    if (mode == DC_MODE) {
        measureDC(voltage);
    } else if (mode == AC_MODE) {
        measureAC(voltage);
    }

    return voltage;
}

void Voltage::pinsTest() {
    char * message = (char *) malloc(12 * sizeof(char));
    PB_LCD_Clear();

    snprintf(message, 0xC, "100m");
    PB_LCD_WriteString(message, 0xC);
    changeVoltageRange(V_100M_Range);
    wait(10);
    PB_LCD_Clear();
    snprintf(message, 0xC, "1");
    PB_LCD_WriteString(message, 0xC);
    changeVoltageRange(V_1_Range);
    wait(10);
    PB_LCD_Clear();
    snprintf(message, 0xC, "5");
    PB_LCD_WriteString(message, 0xC);
    changeVoltageRange(V_5_Range);
    wait(10);
    PB_LCD_Clear();
    snprintf(message, 0xC, "10");
    PB_LCD_WriteString(message, 0xC);
    changeVoltageRange(V_10_Range);
    wait(10);
}