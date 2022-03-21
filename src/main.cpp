#include "mbed.h"

#include "./modes.hpp"

Serial serial(USBTX, USBRX);

DigitalIn button(BUTTON1);

int main() {
    serial.printf("Multimeter starting\n");
    int mode = DC_MODE;

    // Initialise modes selector
    Modes modes;
    modes.initModes();

    while(true) {
        // TODO: Debounce button
        if (button == 1) {
            modes.switchMode(&mode);
        }
    }

    return 0;
}