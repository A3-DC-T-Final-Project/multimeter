#include "ST7066U.hpp"

#include "mbed.h"

ST7066U::ST7066U(PinName rs, PinName rw, PinName e, PinName oe, PinName d0,
                 PinName d1, PinName d2, PinName d3, PinName d4, PinName d5,
                 PinName d6, PinName d7, bool cursorOn, bool cursorBlink)
    : _rs(rs),
      _rw(rw),
      _e(e),
      _oe(oe),
      _d0(d0),
      _d1(d1),
      _d2(d2),
      _d3(d3),
      _d4(d4),
      _d5(d5),
      _d6(d6),
      _d7(d7),
      _cursorsOn{cursorOn},
      _cursorBlink{cursorBlink} {

    // Set all pins as outputs
    _rs.output();
    _rw.output();
    _e.output();
    _oe.output();
    _d0.output();
    _d1.output();
    _d2.output();
    _d3.output();
    _d4.output();
    _d5.output();
    _d6.output();
    _d7.output();

    // Enable LCD output
    _oe.write(1);

    // Default all pins to ground but read !write
    _rs.write(0);
    _rw.write(1);
    _e.write(0);
    _d0.write(0);
    _d1.write(0);
    _d2.write(0);
    _d3.write(0);
    _d4.write(0);
    _d5.write(0);
    _d6.write(0);
    _d7.write(0);

    // Store pointers to data pins
    pins = (DigitalInOut **)malloc(DATA_PINS * sizeof(DigitalInOut *));
    pins[0] = &(_d0);
    pins[1] = &(_d1);
    pins[2] = &(_d2);
    pins[3] = &(_d3);
    pins[4] = &(_d4);
    pins[5] = &(_d5);
    pins[6] = &(_d6);
    pins[7] = &(_d7);

    thread_sleep_for(100);  // 100 ms

    reset();

    thread_sleep_for(100);  // 100 ms

    // No initial custom characters
    customCounter = 0;
}

void ST7066U::read(bool * busy, uint8_t * address) {
    // Set pins as inputs
    int i;
    for(i = 0; i < DATA_PINS; i++) {
        pins[i]->input();
    }

    // Set instruction register
    _rs.write(0);

    // Enable read mode (write is active low)
    _rw.write(1);

    // Start enable signal
    _e.write(1);
    wait_us(OPERATION_TIME_US);

    // Read busy flag
    if (busy != NULL)
        (*busy) = _d7.read();

    // Read address
    if (address != NULL) {
        (*address) = 0;
        for(i = 0; i < (DATA_PINS - 1); i++) {
            (*address) |= (pins[i]->read() << i);
        }
    }

    // Bring enable low
    _e.write(0);
    wait_us(OPERATION_TIME_US);

    // Reset pins as output
    for(i = 0; i < DATA_PINS; i++) {
        pins[i]->output();
    }
}

void ST7066U::write(bool instruction) {
    bool busy = true;
    while(busy) {read(&busy, NULL);} // Do nothing until not busy

    int i;
    // Enable write mode (write is active low)
    _rw.write(0);
    // Select instruction register based on argument
    _rs.write(!instruction);
    // Load data to write on pin
    for (i = 0; i < DATA_PINS; i++) {
        pins[i]->write(data[i]);
    }

    // Start enable signal
    _e.write(1);
    wait_us(OPERATION_TIME_US);
    // Bring enable low
    _e.write(0);
    wait_us(OPERATION_TIME_US);
}

void ST7066U::resetData() {
    int i;
    for (i = 0; i < DATA_PINS; i++) {
        data[i] = 0;
    }
}

void ST7066U::clear() {
    resetData();
    data[0] = 1;
    write(true);

    // Clear takes 1.52 ms to execute
    thread_sleep_for(2);  // 2 ms
}

void ST7066U::reset() {
    // Set 8 bit mode
    resetData();
    data[3] = 1;
    data[4] = 1;
    data[5] = 1;
    write(true);
    write(true);  // Same instrcution twice as per datasheet

    // Set display on
    resetData();
    data[0] = _cursorBlink;
    data[1] = _cursorsOn;
    data[2] = 1;
    data[3] = 1;
    write(true);

    // Clear
    clear();

    // Entry mode set
    resetData();
    data[1] = 1;
    data[2] = 1;
    write(true);
}

void ST7066U::printString(std::string message) {
    size_t message_sz = message.size();

    // Write characters
    unsigned int i;
    for (i = 0; i < message_sz; i++) {
        resetData();
        // Get bit values from char
        int j;
        for (j = 0; j < DATA_PINS; j++) {
            // Get charachter at index
            unsigned char c = message.at(i);
            // Get values of the 8 bits that make up the char
            // In ~most~ systems, a u char is 8 bits.
            data[j] = c & (1 << j);
        }
        // Write data
        write(false);
    }
}

void ST7066U::firstLine() {
    // Set Display RAM (DDRAM) Address to 00H
    resetData();
    data[7] = 1;
    write(true);
}

void ST7066U::clearLine(uint8_t address, uint8_t lastAddress) {
    uint8_t blank = 0x20;

    /* Go to DDRAM Address [address],
     * Wite 20H till [lastAddress] Address reached */
    uint8_t _lastAddress = lastAddress;
    uint8_t _address = address;

    while(_address != _lastAddress) {
        // Set address
        resetData();
        data[7] = 1;
        uint8_t i;
        for(i = 0; i < 7; i++) {
            data[i] = (_address >> i) & 1;
        }
        write(true);

        // Write 20H
        resetData();
        for(i = 0; i < DATA_PINS; i++) {
            data[i] = (blank >> i) & 1;
        }
        write(false);
        _address++;
    }

    // Return to position 0 on line
    if (address == 0)
        firstLine();
    else
        secondLine();
}

void ST7066U::clearFirstLine() {
    // Line 0 starts at 00H and ends at 27H
    clearLine(0, 0x27);
}

void ST7066U::secondLine() {
    // Set Display RAM (DDRAM) Address to 40H
    resetData();
    data[7] = 1;
    data[6] = 1;
    write(true);
}

void ST7066U::clearSecondLine() {
    // Line 0 starts at 40H and ends at 67H
    clearLine(0x40, 0x67);
}

void ST7066U::helloWorld() {
    std::string hello = "Hello World!!";

    clear();

    printString(hello);
}

void ST7066U::setCGRAMAddress(uint8_t address) {
    // Set CGRAM to Address [address]
    resetData();
    data[6] = 1;
    uint8_t i;
    for(i = 0; i < 6; i++) {
        data[i] = (address >> i) & 1;
    }
    write(true);
}

void ST7066U::setDDRAMAddress(uint8_t address) {
    // Set DDRAM to Address [address]
    resetData();
    data[7] = 1;
    uint8_t i;
    for(i = 0; i < 6; i++) {
        data[i] = (address >> i) & 1;
    }
    write(true);
}

uint8_t ST7066U::storeCustomChar(uint8_t custom[8][5]) {
    // Get current DDRAM Address
    uint8_t prevAddress = 0;
    read(NULL, &prevAddress);

    /* CGRAM Address starts at 40H. In 5x8 mode,
     * each character takes 8 addresses in RAM.
     * For instance:
     * 1st custom character: From 40H to 47H
     * 2nd custom character: From 48H to 4FH
     * 3rd custom character: From 50H to 57H
     */
    uint8_t address = 0x40 + (7 * customCounter) + customCounter;
    setCGRAMAddress(address);

    uint8_t i, j;
    for(i = 0; i < 8; i++) {
        resetData();
        for(j = 0; j < 5; j++) {
            data[j] = custom[i][j];
        }

        write(false);
    }

    // Restore DDRAM Address
    setDDRAMAddress(prevAddress);

    uint8_t prevCustomCounter = customCounter;

    // Increment custom counter
    customCounter++;

    // Return address where custom char is saved. In 5x8 mode,
    // address can be 0 to 7.
    return prevCustomCounter;
}

void ST7066U::writeCustomChar(uint8_t custom) {
    resetData();
    uint8_t i;
    // Write custom character stored at address [custom]
    for(i = 0; i < DATA_PINS; i++) {
        data[i] = (custom >> i) & 1;
    }

    write(false);
}