#ifndef MBED_ST7066U
#define MBED_ST7066U

#include "mbed.h"

#define DATA_PINS 8
#define OPERATION_TIME_US 37

class ST7066U {
   public:
    ST7066U(PinName rs, PinName rw, PinName e, PinName oe, PinName d0,
            PinName d1, PinName d2, PinName d3, PinName d4, PinName d5,
            PinName d6, PinName d7, bool cursorOn, bool cursorBlink);
    void clear();
    void printString(std::string message);
    void firstLine();
    void secondLine();
    void helloWorld();

   private:
    DigitalInOut _rs, _rw, _e, _oe, _d0, _d1, _d2, _d3, _d4, _d5, _d6, _d7;
    DigitalInOut** pins;
    bool _cursorsOn, _cursorBlink;
    int data[DATA_PINS];
    bool isBusy();
    void resetData();
    void write(bool instruction);
    void reset();
};

#endif /* MBED_ST7066U */