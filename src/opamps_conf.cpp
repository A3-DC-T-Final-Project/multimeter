#include "opamps_conf.hpp"
#include "current.hpp"
#include "modes.hpp"
#include "voltage.hpp"

void OpAmpsConf::initOpampConf(int range, int mode, float VDDA) {
    float Rf, Rin, upperRange, lowerRange, upperFirstStage, lowerFirstStage;
    switch (mode) {
        default:
        case AC_MODE:
        case DC_MODE:
            switch (range) {
                default:
                case V_10_Range:
                    Rf = 27e3;
                    Rin = 100e3;
                    upperRange = 5;
                    lowerRange = -5;
                    upperFirstStage = 10;
                    lowerFirstStage = -10;
                    break;
                case V_5_Range:
                    Rf = 27e3;
                    Rin = 51e3;
                    upperRange = 1;
                    lowerRange = -1;
                    upperFirstStage = 5;
                    lowerFirstStage = -5;
                    break;
                case V_1_Range:
                    Rf = 270e3;
                    Rin = 100e3;
                    upperRange = 100e-3;
                    lowerRange = -100e-3;
                    upperFirstStage = 1;
                    lowerFirstStage = -1;
                    break;
                case V_100M_Range:
                    Rf = 750e3;
                    Rin = 30e3;
                    upperRange = 100e-3;
                    lowerRange = -100e-3;
                    upperFirstStage = upperRange;
                    lowerFirstStage = lowerRange;
                    break;
            }
            break;
        case I_MODE:
            switch (range) {
                default:
                case I_100:
                    Rf = 27e3;
                    Rin = 1e3;
                    upperRange = 50e-3;
                    lowerRange = -50e-3;
                    upperFirstStage = 100e-3;
                    lowerFirstStage = -100e-3;
                    break;
                case I_50:
                    Rf = 27e3;
                    Rin = 510;
                    upperRange = 25e-3;
                    lowerRange = -25e-3;
                    upperFirstStage = 50e-3;
                    lowerFirstStage = -50e-3;
                    break;
                case I_25:
                    Rf = 120e3;
                    Rin = 1.1e3;
                    upperRange = 10e-3;
                    lowerRange = -10e-3;
                    upperFirstStage = 25e-3;
                    lowerFirstStage = -25e-3;
                    break;
                case I_10:
                    Rf = 270e3;
                    Rin = 1e3;
                    upperRange = 10e-3;
                    lowerRange = -10e-3;
                    upperFirstStage = upperRange;
                    lowerFirstStage = lowerRange;
                    break;
            }
            break;
    }

    upperFirstStage *= -(Rf / Rin);
    lowerFirstStage *= -(Rf / Rin);
    upperRange *= -(Rf / Rin);
    lowerRange *= -(Rf / Rin);

    expectedUpper = (VDDA - upperFirstStage) / 2;
    expectedLower = (VDDA - lowerFirstStage) / 2;
    rangeUpperBound = (VDDA - upperRange) / 2;
    rangeLowerBound = (VDDA - lowerRange) / 2;
}

float OpAmpsConf::getExpectedUpper() {
    return expectedUpper;
}

float OpAmpsConf::getExpectedLower() {
    return expectedLower;
}

float OpAmpsConf::getRangeUpperBound() {
    return rangeUpperBound;
}

float OpAmpsConf::getRangeLowerBound() {
    return rangeLowerBound;
}