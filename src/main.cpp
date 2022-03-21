#include "mbed.h"

#include "modes.hpp"

extern "C" {
    #include <PB_LCD_Drivers.h>
}

Serial serial(USBTX, USBRX);

DigitalIn button(BUTTON1);

int main() {
    serial.printf("Multimeter starting\n");
    int mode = DC_MODE;

    // Initialise modes selector
    Modes modes;
    modes.initModes();

    // Initialise LCD
    PB_LCD_Init();
    PB_LCD_Clear();

    while(true) {
        // TODO: Debounce button
        if (button == 1) {
            modes.switchMode(&mode);
        }

        // Print mode info to LCD
        PB_LCD_GoToXY(0, 0);
        char * message = (char *) malloc(12 * sizeof(char));
        switch (mode) {
            default:
            case DC_MODE:
                snprintf(message, 0xC, "DC Voltage:");
                PB_LCD_WriteString(message, 0xC);
                break;
            case AC_MODE:
                snprintf(message, 0xC, "AC Voltage:");
                PB_LCD_WriteString(message, 0xC);
                break;
            case I_MODE:
                snprintf(message, 0xC, "Current:");
                PB_LCD_WriteString(message, 0xC);
                break;
            case R_MODE:
                snprintf(message, 0xC, "Resistance:");
                PB_LCD_WriteString(message, 0xC);
                break;
        }
        free(message);
    }

    return 0;
}