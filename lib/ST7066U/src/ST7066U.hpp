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
    void clearFirstLine();
    void clearSecondLine();
    void secondLine();
    uint8_t storeCustomChar(uint8_t custom[8][5]);
    void helloWorld();
    void writeCustomChar(uint8_t custom);
    uint8_t smiley[8][5] = {
        {0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0}
    };

   private:
    DigitalInOut _rs, _rw, _e, _oe, _d0, _d1, _d2, _d3, _d4, _d5, _d6, _d7;
    DigitalInOut** pins;
    bool _cursorsOn, _cursorBlink;
    uint8_t customCounter;
    uint8_t data[DATA_PINS];
    void resetData();
    void write(bool instruction);
    void read(bool * busy, uint8_t * address);
    void reset();
    void clearLine(uint8_t address, uint8_t lastAddress);
    void setCGRAMAddress(uint8_t address);
    void setDDRAMAddress(uint8_t address);
};

#endif /* MBED_ST7066U */