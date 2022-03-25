#include "mbed.h"

#include "modes.hpp"
#include "voltage.hpp"
#include "common.hpp"

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

void Voltage::calculateExpectedVoltages() {
    switch(range) {
        default:
        case V_10_Range:
            float Rf = 27e3;
            float Rin = 100e3;
            float upperFirstStage = 10;
            float lowerFirstStage = -10;
            float upperRange = 5;
            float lowerRange = -5;

            upperFirstStage *= -(Rf/Rin);
            lowerFirstStage *= -(Rf/Rin);
            upperRange *= -(Rf/Rin);
            lowerRange *= -(Rf/Rin);

            float VDDA = getVDDA();

            expectedUpper = (VDDA - upperFirstStage) / 2;
            expectedLower = (VDDA - lowerFirstStage) / 2;
            rangeUpperBound = (VDDA - upperRange) / 2;
            rangeLowerBound = (VDDA - lowerRange) / 2;

            break;
    }
}

void Voltage::changeVoltageRange(int range) {
    switch (range) {
        case V_100M_Range:
            upperVoltage = 100;
            lowerVoltage = -100;
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

void Voltage::measureDC(char * voltage) {
    int i;
    float total = 0;
    for(i = 0; i < 1000; i++) {
        total += vIn->read();

        /* https://www.st.com/resource/en/datasheet/dm00037051.pdf
         * ADC clock should be 30MHz. (Page 133)
         * We need to sample a slowest wave at 100Hz.
         * So to cover the full length of the wave:
         * min tCONV = 0.5us, max tCONV 16.4us (Page 134)
         * average tCONV = (16.4u + 0.5u)/2 = 8.45us
         * 100Hz = Full wave takes 10ms
         * 1000 samples = 8.45us * 1000 = 8.45ms
         * time to add = 10ms - 8.45ms = 1.55ms, round up to 2ms just because *real world*
         * delay = 2ms/1000 = 2us
         */

        wait_us(2);
    }

    total /= 1000; // Average of our reading
    
    float vref = getVREF();

    calculateExpectedVoltages();

    lowerBound = Common::calculateBound(vref, expectedLower);
    upperBound = Common::calculateBound(vref, expectedUpper);

    float calculatedVoltage = Common::map(total, lowerBound, upperBound, lowerVoltage, upperVoltage);

    if(calculatedVoltage > rangeLowerBound && calculatedVoltage < rangeUpperBound) {
        range -= 1;
        changeVoltageRange(range);
    }

    if (range == V_100M_Range)
        snprintf(voltage, 0x11, "%.5lfmV", calculatedVoltage);
    else
        snprintf(voltage, 0x11, "%.5lfV", calculatedVoltage);
}

char * Voltage::measureVoltage(int mode) {
    char * voltage = (char *) malloc(12 * sizeof(char));
    snprintf(voltage, 0xC, "Placeholder");

    if (mode == DC_MODE) {
        measureDC(voltage);
    } else if (mode == AC_MODE) {
        // measureAC(voltage);
    }

    return voltage;
}

float Voltage::getVREF() {
    AnalogIn * vref = new AnalogIn(ADC_VREF);

    float reference = vref->read();

    free(vref);
    return(reference);

    /* float maxADC = 1.0;

    float threeVolts = (maxADC * 1.21)/reference;

    PB_LCD_Clear();
    char * message = (char *) malloc(0x11 * sizeof(char));
    snprintf(message, 0x11, "1.0: %.4fV", threeVolts);
    PB_LCD_WriteString(message, 0x11);

    PB_LCD_GoToXY(0, 1);
    snprintf(message, 0x11, "%.4f: 1.21V", reference);
    PB_LCD_WriteString(message, 0x11);
    free(message); */
}

float Voltage::getVDDA() {
    float vref = getVREF();

    float threeVolts = 1.0;

    // VREFINT has a typical value of 1.21V
    // Page 139 https://www.st.com/resource/en/datasheet/dm00037051.pdf
    threeVolts = 1.21 / vref;

    return threeVolts;
}