#include "mbed.h"

#include "modes.hpp"
#include "voltage.hpp"

extern "C" {
    #include <PB_LCD_Drivers.h>
}

Serial serial(USBTX, USBRX);

DigitalIn button(BUTTON1);

int main() {
    serial.printf("Multimeter starting\n");
    int mode = DC_MODE;

    int lastPress = 0;

    // Initialise modes selector
    Modes modes;
    modes.initModes();

    // Initialise voltage reading
    Voltage voltage;
    voltage.initVoltage();

    // Initialise LCD
    PB_LCD_Init();

    while(true) {
        // Clear LCD
        PB_LCD_Clear();

        if (button == 1 && lastPress == 0) {
            modes.switchMode(&mode);
            lastPress = 1;
        }

        if(button == 0 && lastPress == 1) {
            lastPress = 0;
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

        voltage.pinsTest();

        wait_ms(200);
    }

    return 0;
}