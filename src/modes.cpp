#include "modes.hpp"

void Modes::initModes() {
    modeA0 = new DigitalOut(PB_5);
    modeA1 = new DigitalOut(PB_7);
}

void Modes::switchMode(int * mode) {
    (*mode)++;

    if ((*mode) == (R_MODE + 1)) {
        (*mode) = DC_MODE;
    }

    // TODO: Hard-wire to ground
    modeA1->write(0);

    switch(*mode) {
        default:
        case R_MODE:
		case AC_MODE:
		case DC_MODE:
			modeA0->write(0);
			break;
		case I_MODE:
			modeA0->write(1);
			break;
    }
}