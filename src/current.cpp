#include "mbed.h"

#include "current.hpp"
#include "voltage.hpp"
#include "common.hpp"

void Current::initCurrent(Serial * serial, Voltage * voltage) {
    iRangeA0 = new DigitalOut(PE_5);
    iRangeA1 = new DigitalOut(PE_6);
    iIn = new AnalogIn(PC_4);

    currentSerial = serial;
    voltage_ptr = voltage;

    range = I_100;
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
            iRangeA1->write(1);
        break;
    }
}

void Current::calculateExpectedCurrents() {
    // 100m values as default
    float Rf = 27e3;
    float Rin = 1e3;
    float upperRange = 50e-3;
    float lowerRange = -50e-3;
    float upperFirstStage = upperVoltage;
    float lowerFirstStage = lowerVoltage;
    switch(range) {
        default:
        case I_100:
            Rf = 27e3;
            Rin = 1e3;
            upperRange = 50e-3;
            lowerRange = -50e-3;
            break;
        case I_50:
            Rf = 27e3;
            Rin = 510;
            upperRange = 50e-3;
            lowerRange = -50e-3;
            break;
        case I_25:
            Rf = 120e3;
            Rin = 1.1e3;
            upperRange = 25e-3;
            lowerRange = -25-3;
            break;
        case I_10:
            Rf = 270e3;
            Rin = 1e3;
            // Set upper and lower range to same value
            upperRange = 10e-3;
            lowerRange = -10e-3;
    }

    upperFirstStage *= -(Rf/Rin);
    lowerFirstStage *= -(Rf/Rin);
    upperRange *= -(Rf/Rin);
    lowerRange *= -(Rf/Rin);

    float VDDA = voltage_ptr->getVDDA();

    expectedUpper = (VDDA - upperFirstStage) / 2;
    expectedLower = (VDDA - lowerFirstStage) / 2;
    rangeUpperBound = (VDDA - upperRange) / 2;
    rangeLowerBound = (VDDA - lowerRange) / 2;
}

char * Current::measureCurrent() {
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

    calculateExpectedCurrents();

    lowerBound = Common::calculateBound(vref, expectedLower);
    upperBound = Common::calculateBound(vref, expectedUpper);

    float calculatedCurrent = Common::map(total, lowerBound, upperBound, lowerVoltage, upperVoltage);
    float calculatedTotal = total * vdda;

    char * current = (char *) malloc(0x10 * sizeof(char));

    // Multiply by 1000 to have sig fifures before floating point.
    snprintf(current, 0x11, "%.5lfmA", calculatedCurrent * 1000);


    if((calculatedTotal > rangeLowerBound && calculatedTotal < rangeUpperBound)
        && range != I_10) {
        range -= 1;
        changeCurrentRange(range);
    } else if (((calculatedTotal > expectedUpper) || (calculatedTotal < expectedLower)) && range != I_100) {
        range += 1;
        changeCurrentRange(range);
    }

    return current;
}