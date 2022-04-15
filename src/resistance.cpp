#include "resistance.hpp"

#include "mbed.h"
#include "modes.hpp"
#include "voltage.hpp"

void Resistance::initResistance(Serial *serial, Voltage *voltage) {
    rRangeA0 = new DigitalOut(PB_4);
    rRangeA1 = new DigitalOut(PB_5);

    voltage_ptr = voltage;
    resistanceSerial = serial;

    range = R_1M;
    changeCurrentRange(range);
}

void Resistance::changeCurrentRange(int range) {
    switch (range) {
        default:
        case R_1M:
            rRangeA0->write(0);
            rRangeA1->write(1);
            break;
        case R_100U:
            rRangeA0->write(1);
            rRangeA1->write(0);
            break;
        case R_10U:
            rRangeA0->write(0);
            rRangeA1->write(0);
            break;
    }
}

char * Resistance::measureResistance() {
    float voltage = 0;
    voltage_ptr->measureVoltage(DC_MODE, &voltage);

    float calculatedResistance = 0;

    switch (range) {
        default:
        case R_1M:
            calculatedResistance = (voltage / 1e-3);
            break;
        case R_100U:
            calculatedResistance = (voltage / 1e-4);
            break;
        case R_10U:
            calculatedResistance = (voltage / 1e-5);
            break;
    }

    /* Experiment needed to work out auto ranging logic */

    char * resistance = (char *) malloc(0x10 * sizeof(char));
    snprintf(resistance, 0x10, "%.5f Ohms", calculatedResistance);

    return resistance;
}