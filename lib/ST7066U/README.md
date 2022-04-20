# ST7066U mbed library
This is a library to interface an mbed supported board with a 16 by 2 LCD display using the [ST7066U](https://newhavendisplay.com/content/app_notes/ST7066U.pdf) display driver.

Tested on the [STM32F407 Discovery board](https://www.st.com/en/evaluation-tools/stm32f4discovery.html) running [PlatformIO mbed 5](https://docs.platformio.org/en/latest/frameworks/mbed.html#framework-mbed) with the [discof407_vg](https://docs.platformio.org/en/latest/boards/ststm32/disco_f407vg.html) configuration.

# Usage
## Initialisation
Construct an ST7066U object:
```cpp
ST7066U st7066u(PB_0, PB_1, PB_2, PA_15, PD_0, PD_1, PD_2, PD_3, PD_4, PD_5,
                    PD_6, PD_7, false, true);
```
Then use the object to perform any of the below actions.

## Checking everything works and is wired up correctly
```cpp
st7066u.helloWorld();
```

## Clear the display
```cpp
st7066u.clear();
```

## Move to the first line
```cpp
st7066u.firstLine();
```

## Move to the second line
```cpp
st7066u.secondLine();
```

## Printing a string
```cpp
st7066u.printString("A constant string");
```
Or
```cpp
std::string message = "A variable string";
st7066u.printString(message);
```

# TO DO
 * Provide a function to only clear one line.
 * Provide a function to move to a specific location on a line.