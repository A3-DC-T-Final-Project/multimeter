#include "voltage.hpp"

#include "common.hpp"
#include "mbed.h"
#include "modes.hpp"
#include "opamps_conf.hpp"

void Voltage::initVoltage(Serial *serial, AnalogIn *input) {
    vRangeA0 = new DigitalOut(PE_3);
    vRangeA1 = new DigitalOut(PE_4);
    vIn = input;

    voltageSerial = serial;

    range = V_10_Range;
    changeVoltageRange(range);
}

void Voltage::changeVoltageRange(int range) {
#if (VOLTAGE_DEBUG == 1)
    voltageSerial->printf("Using range: %d\n", range);
#endif
    switch (range) {
        case V_100M_Range:
            upperVoltage = 100e-3;
            lowerVoltage = -100e-3;
            vRangeA0->write(0);
            vRangeA1->write(0);
            break;
        case V_1_Range:
            upperVoltage = 1;
            lowerVoltage = -1;
            vRangeA0->write(1);
            vRangeA1->write(0);
            break;
        case V_5_Range:
            upperVoltage = 5;
            lowerVoltage = -5;
            vRangeA0->write(0);
            vRangeA1->write(1);
            break;
        default:
        case V_10_Range:
            upperVoltage = 10;
            lowerVoltage = -10;
            vRangeA0->write(1);
            vRangeA1->write(1);
            break;
    }
}

void Voltage::measureDC(char *voltage, OpAmpsConf *opAmpsConf,
                        float *voltageForResistance) {
    int i;
    float total = 0;
    for (i = 0; i < 1000; i++) {
        total += vIn->read();

        /* https://www.st.com/resource/en/datasheet/dm00037051.pdf
         * ADC clock should be 30MHz. (Page 133)
         * We need to sample a slowest wave at 100Hz.
         * So to cover the full length of the wave:
         * min tCONV = 0.5us, max tCONV 16.4us (Page 134)
         * average tCONV = (16.4u + 0.5u)/2 = 8.45us
         * 100Hz = Full wave takes 10ms
         * 1000 samples = 8.45us * 1000 = 8.45ms
         * time to add = 10ms - 8.45ms = 1.55ms, round up to 2ms just because
         * *real world* delay = 2ms/1000 = 2us
         */

        wait_us(2);
    }

    total /= 1000;  // Average of our reading

    float vref = getVREF();
    float vdda = getVDDA();

    opAmpsConf->initOpampConf(range, DC_MODE, getVDDA());

    float expectedLower = opAmpsConf->getExpectedLower();
    float expectedUpper = opAmpsConf->getExpectedUpper();
    float rangeLowerBound = opAmpsConf->getRangeLowerBound();
    float rangeUpperBound = opAmpsConf->getRangeUpperBound();

    float lowerBound = CommonUtils::calculateBound(vref, expectedLower);
    float upperBound = CommonUtils::calculateBound(vref, expectedUpper);

    float calculatedVoltage = CommonUtils::map(total, lowerBound, upperBound,
                                               lowerVoltage, upperVoltage);
    float calculatedTotal = total * vdda;

    if (voltageForResistance == NULL) {
        if (range == V_100M_Range)
            snprintf(voltage, 0x11, "%.3lfmV", calculatedVoltage * 1000);
        else
            snprintf(voltage, 0x11, "%.3lfV", calculatedVoltage);
    } else {
        (*voltageForResistance) = calculatedVoltage;
    }

    if ((calculatedTotal > rangeLowerBound &&
         calculatedTotal < rangeUpperBound) &&
        range != V_100M_Range) {
        range -= 1;
        changeVoltageRange(range);
    } else if (((calculatedTotal > expectedUpper) ||
                (calculatedTotal < expectedLower)) &&
               range != V_10_Range) {
        range += 1;
        changeVoltageRange(range);
    }
}

void Voltage::measureAC(char *voltage, OpAmpsConf *opAmpsConf) {
    // Only use 10V range for now

    changeVoltageRange(V_10_Range);

    float vref = getVREF();

    opAmpsConf->initOpampConf(range, DC_MODE, getVDDA());

    float expectedLower = opAmpsConf->getExpectedLower();
    float expectedUpper = opAmpsConf->getExpectedUpper();

    float lowerBound = CommonUtils::calculateBound(vref, expectedLower);
    float upperBound = CommonUtils::calculateBound(vref, expectedUpper);

    int i;
    double sumOfSquares = 0;
    for (i = 0; i < 1000; i++) {
        float ADCReading = vIn->read();
        float instantaneousVoltage = CommonUtils::map(
            ADCReading, lowerBound, upperBound, lowerVoltage, upperVoltage);
        sumOfSquares += pow(instantaneousVoltage, 2);

        // As per DC reading.
        wait_us(2);
    }

    sumOfSquares /= 1000;

    double result = sqrt(sumOfSquares);
    if(result > 1.15) {
        // Offset determined experimentally
        result -= 0.4;
    }

    snprintf(voltage, 0x11, "%.3lf Vrms", result);
}

char *Voltage::measureVoltage(int mode, float *voltageForResistance) {
    char *voltage = (char *)malloc(0x11 * sizeof(char));
    snprintf(voltage, 0xC, "Placeholder");

    OpAmpsConf *opAmpsConf = new OpAmpsConf();

    if (mode == DC_MODE) {
        measureDC(voltage, opAmpsConf, voltageForResistance);
    } else if (mode == AC_MODE) {
        measureAC(voltage, opAmpsConf);
    }

    free(opAmpsConf);
    return voltage;
}

float Voltage::getVREF() {
    AnalogIn *vref = new AnalogIn(ADC_VREF);

    float reference = vref->read();

    free(vref);
    return (reference);
}

float Voltage::getVDDA() {
    float vref = getVREF();

    float threeVolts = 1.0;

    // VREFINT has a typical value of 1.21V
    // Page 139 https://www.st.com/resource/en/datasheet/dm00037051.pdf
    threeVolts = 1.21 / vref;

    return threeVolts;
}