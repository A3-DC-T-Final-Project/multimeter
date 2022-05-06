#include "mbed.h"

#include "modes.hpp"
#include "current.hpp"
#include "voltage.hpp"
#include "common.hpp"
#include "opamps_conf.hpp"

void Current::initCurrent(Serial * serial, Voltage * voltage, AnalogIn * input) {
    iRangeA0 = new DigitalOut(PE_5);
    iRangeA1 = new DigitalOut(PE_6);
    iIn = input;

    currentSerial = serial;
    voltage_ptr = voltage;

    range = I_100;
    changeCurrentRange(range);
}

void Current::changeCurrentRange(int range) {
    switch (range) {
        default:
        case I_100:
            upperVoltage = 100e-3;
            lowerVoltage = -100e-3;
            iRangeA0->write(1);
            iRangeA1->write(1);
        break;
        case I_50:
            upperVoltage = 50e-3;
            lowerVoltage = -50e-3;
            iRangeA0->write(0);
            iRangeA1->write(1);
        break;
        case I_25:
            upperVoltage = 25e-3;
            lowerVoltage = -25e-3;
            iRangeA0->write(1);
            iRangeA1->write(0);
        break;
        case I_10:
            upperVoltage = 10e-3;
            lowerVoltage = -10e-3;
            iRangeA0->write(0);
            iRangeA1->write(0);
        break;
    }
}

char * Current::measureCurrent(bool * hasAutoRanged) {
    OpAmpsConf * opAmpsConf = new OpAmpsConf();

    int i;
    float total = 0;
    for(i = 0; i < 1000; i++) {
        total += iIn->read();

        // As per DC, might be useless here?
        wait_us(2);
    }

    total /= 1000;

    float vref = voltage_ptr->getVREF();
    float vdda = voltage_ptr->getVDDA();

    opAmpsConf->initOpampConf(range, I_MODE, voltage_ptr->getVDDA());

    float expectedLower = opAmpsConf->getExpectedLower();
    float expectedUpper = opAmpsConf->getExpectedUpper();
    float rangeLowerBound = opAmpsConf->getRangeLowerBound();
    float rangeUpperBound = opAmpsConf->getRangeUpperBound();

    float lowerBound = CommonUtils::calculateBound(vref, expectedLower);
    float upperBound = CommonUtils::calculateBound(vref, expectedUpper);

    float calculatedCurrent = CommonUtils::map(total, lowerBound, upperBound, lowerVoltage, upperVoltage);
    float calculatedTotal = total * vdda;

    // Current has 1mV offset
    if(range != I_25 && range != I_10) {
        calculatedCurrent *= 1000;
        calculatedCurrent += 1;
        calculatedCurrent /= 1000;
    }

    char * current = (char *) malloc(0x10 * sizeof(char));

    // Multiply by 1000 to have sig fifures before floating point.
    snprintf(current, 0x11, "%.3lfmA", calculatedCurrent * 1000);

    if((calculatedTotal > rangeLowerBound && calculatedTotal < rangeUpperBound)
        && range != I_10) {
        range -= 1;
        changeCurrentRange(range);

        (*hasAutoRanged) = true;
    } else if (((calculatedTotal > expectedUpper) || (calculatedTotal < expectedLower)) && range != I_100) {
        range += 1;
        changeCurrentRange(range);

        (*hasAutoRanged) = true;
    }

    return current;
}