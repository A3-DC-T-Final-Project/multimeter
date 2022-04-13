#ifndef _OPAMPS_CONF_H_
#define _OPAMPS_CONF_H_

class OpAmpsConf {
    private:
        float expectedUpper, expectedLower, rangeUpperBound, rangeLowerBound;
    public:
        void initOpampConf(int range, int mode, float VDDA);
        float getExpectedUpper();
        float getExpectedLower();
        float getRangeUpperBound();
        float getRangeLowerBound();
};

#endif /*_OPAMPS_CONF_H_ */