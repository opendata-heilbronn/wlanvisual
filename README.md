# WLAN Visual

This project was started during an event at CodeforHN.

https://www.meetup.com/de-DE/codeforhn/events/248306628/

It uses a NodeMCU (ESP-2866) for WLAN connectivity and an NeoPixel FeatherWing - 4x8 RGB LED Module for visualisation. We also added a small I2C OLED Display for debugging
and additional information.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

You need a running Arduino IDE with the ESP-2866 board enabled. You find a good
tutorial (in german language) at http://blog.opendatalab.de/codeforbuga/2016/07/02/arduino-ide-mit-nodemcu-esp8266

The project uses the following libraries (installed via library manager):

```
- Adafruit GFX (for OLED Display)
- Adafruit SSD1306 (for OLED Display)
- ESP8266 & ESP8266WiFi
- FastLED
```

### Installing

Just select your board and communication port, then compile and flash the file
with the Arduino IDE.

## Schematic

TODO ;)

Here some text:
```
D0 - Internal LED
D1 - OLED I2C Data
D2 - OLED I2C Clock
D5 - NeoPixel Input

OLED might use 3V3! Look at your pinout and specs ;)
NodeMCU is powered with its USB connector from 5V Power Bank
NodeMCU provides 5V for NeoPixel
NeoPixel data pin is used at 3V3.
```

## Authors

* **Tim Oldenbuerger** - *Initial work*
* **Franz Imschweiler** - *Hardware selection, color selection and Case building*
* **Leon Hellmich** - *Photographs, heatmap generation*
* **Leandro Spaeth** - *OLED History Graph*
* **https://github.com/patagonaa** - *Code review, mathematics*


## License

This project is licensed under the CC BY-NC license - see the [LICENSE.md](LICENSE.md) file for details or visit https://creativecommons.org/licenses/by-nc/4.0
