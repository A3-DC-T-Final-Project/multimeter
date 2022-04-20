#ifndef _MODES_H_
#define _MODES_H_

#include "mbed.h"

#define DC_MODE 0
#define AC_MODE 1
#define I_MODE 2
#define R_MODE 3

class Modes {
    private:
        DigitalOut * modeA0;
    public:
        void initModes();
        void switchMode(int * mode);
};


#endif /* _MODES_H_ */