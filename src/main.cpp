#include "mbed.h"

#include "modes.hpp"
#include "voltage.hpp"
#include "current.hpp"
#include "resistance.hpp"
#include "common.hpp"

#include <ST7066U.hpp>

DigitalIn button(BUTTON1);

int main() {
    Serial * serial = new Serial(USBTX, USBRX);

    serial->printf("Multimeter starting\n");
    int mode = DC_MODE;

    int lastPress = 0;

    // Initialise LCD
    ST7066U st7066u(PB_0, PB_1, PB_2, PA_15, PD_0, PD_1, PD_2, PD_3, PD_4, PD_5,
                    PD_6, PD_7, false, false);

    // Initialise common class (initialises analoginput)
    Common * common = new Common();
    AnalogIn * input = common->initInput();

    // Initialise modes selector
    Modes modes;
    modes.initModes();

    // Initialise voltage reading
    Voltage * voltage = new Voltage();
    voltage->initVoltage(serial, input);

    // Initialise current reading
    Current * current = new Current();
    current->initCurrent(serial, voltage, input);

    // Initialises resistance reading
    Resistance * resistance = new Resistance();
    resistance->initResistance(serial, voltage);

    uint8_t omegaAddr = st7066u.storeCustomChar(resistance->omega);

    while(true) {
        // Clear LCD
        st7066u.clear();

        if (button == 1 && lastPress == 0) {
            modes.switchMode(&mode);
            lastPress = 1;
        }

        if(button == 0 && lastPress == 1) {
            lastPress = 0;
        }

        // Print mode info to LCD
        st7066u.firstLine();
        char * message = (char *) malloc(12 * sizeof(char));
        char * measurement;
        switch (mode) {
            default:
            case DC_MODE:
                snprintf(message, 0xC, "DC Voltage:");
                st7066u.printString(message);
                measurement = voltage->measureVoltage(DC_MODE, NULL);
                st7066u.secondLine();
                st7066u.printString(measurement);
                break;
            case AC_MODE:
                snprintf(message, 0xC, "AC Voltage:");
                st7066u.printString(message);
                measurement = voltage->measureVoltage(AC_MODE, NULL);
                st7066u.secondLine();
                st7066u.printString(measurement);
                break;
            case I_MODE:
                snprintf(message, 0xC, "Current:");
                st7066u.printString(message);
                measurement = current->measureCurrent();
                st7066u.secondLine();
                st7066u.printString(measurement);
                break;
            case R_MODE:
                snprintf(message, 0xC, "Resistance:");
                st7066u.printString(message);
                bool buttonIsPressed = false;
                measurement = resistance->measureResistance(&buttonIsPressed);
                st7066u.secondLine();
                if(buttonIsPressed) {
                    st7066u.printString(measurement);
                    st7066u.writeCustomChar(omegaAddr);
                } else 
                    st7066u.printString("Press button");
                
                break;
        }
        free(message);

        // 200ms
        wait_us(2e5);
    }

    return 0;
}