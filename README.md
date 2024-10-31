# ***espSnap***

## A Http Server bridging Snap4Arduino and ESP32/ESP8266 Arduino boards

**_espSnap_** is a Http Server running on ESP32 or ESP8266 to serve as a bridge between **Snap4Arduino** and Arduino boards. It enables **Snap4Arduino** users to control electronic hardwares/modules connected to the Arduino board.

It follows the concept of [**Firmata Lbirary**](https://docs.arduino.cc/retired/hacking/software/FirmataLibrary/) which allows communication between Arduino boards and host computer. However, this implementation **DO NOT** follow the [**Firmata Protocol**](https://github.com/firmata/protocol).

In this implementation, we used http via the ***url block*** ![**url**](assets/images/url_block_espsnap_local_h20.png) in **Snap4Arduino** as the integration method. With ESP32 or ESP8266 as the preferred Arduino boards we had developed and test it for, **Wi-Fi** is the chosen communication method.

We provide **"Custom Blocks"** in **Snap4Arduino** so that **Snap4Arduino** users will feel the Arduino board as an integrated part of it, and focus on using it instead of learn how to use it.

***espSnap*** can be configured to connect to any local **Wi-Fi** Access Point, or on failure, will start up under SoftAP mode allowing up to 4 host computers to connect to it.

mDNS has been configured, this allows the use of domain name in url instead of IP address. The default mDNS ID is "***espsnap.local***"<BR>

The default Soft AP<BR>
- SSID : **espsnap_xxxxxx** <BR>
    where xxxxxx is the last 6 characters of MAC address<BR>
- PASSWORD : **test1234**<BR>

All mDNS, SSID and PASSWORD can be changed via web browser.

To test connection between host computer and ***espSnap***, use any web browser and type the following url in the address bar<BR>
**http://espsnap.local/**


### Functions implemented in ***espSnap*** are as follows

| Basic Arduino Functions  |
|:------------------------:|
| Digital Input            |
| Digital Output           |
| Analog Input             |
| Analog Output            |
|   - DAC <sup>[1]</sup>   |
|   - PWM                  |
|   - Servo                |
|   - Tone                 |

###### Note [1] : Analog Output - DAC is only available on ESP32


| Additional Functions        | Arduino Parts            |
|:----------------------------|:------------------------|
| I2C OLED                    | I2C OLED SSD1306 128x64 |
| I2C LCD                     | I2C LCD 16x2            |
| Ultrasonic Distance Sensor  | HC-SR04                 |
| Temperature/Humidity Sensor | DTH22                   |
| Temperature Sensor          | DS18B20                 |
| NeoPixel                    | WS2812B                 |
| LDR Light Sensor            | KY-018                  |
| Rotary Encoder              | EC11 / KY-040           |
| Rain Sensor Module          |                         |



#### ESP32 board tested
 - Wemos R1 D32


#### ESP8266 board tested
 - NodeMCU 1.0 (ESP-12E Module)


Latest release [![espSnap release-v1.0.0](/assets/images/github_espSnap_release-1.0.0.svg)](https://github.com/tomoto-my/espsnap/releases/latest/)


<BR>

---
## Custom Block for ***espSnap***

**"Custom Block"** for ***espSnap*** is available in subdirectory `assets/Snap_Library/`

The file `espSnap_Library.xml` contains **"Custom Blocks"** for all the Basic Arduino Functions and Additional Functions listed above.

For detailed example of how to use the **"Custom Block"**, refer to the [Companion Tutorials below](https://github.com/tomoto-my/espSnap/tree/main?tab=readme-ov-file#companion-tutorials-with-examples-available-at).

<BR>

---
## Companion Tutorials with Examples Available at

**Companion Tutorials and Examples : [Nan Hwa Snap4Arduino Training](https://github.com/tomoto-my/nanhwasnap4arduinotraining/)**

This companion tutorial contains 1 section as **_Basic Introduction to Snap_** and 2 sections with examples on using the **"Custom Blocks"** of **_espSnap_** in **Snap4Arduino**

<BR>

---

# Installation of ***espSnap*** on ESP32/ESP8266 board

Download the latest version of ***[espSnap main.zip](https://github.com/tomoto-my/espSnap/archive/refs/heads/main.zip)***

Extract it under your `Arduino` or `Arduino/sketch` subdirectory

Use your Arduino IDE to open the **`espSnap.ino`** Arduino Sketch.

**If you plan to use a local Wi-Fi Access Point, remember to change the SSID and PASSWORD.** Otherwise, the SSID and PASSWORD can be change any time via web browser.

Follow the usual step of selecting the ESP32/ESP8266 board to program and compile and upload.

If some libraries required are missing during compilation, please make sure the list of libraries as listed below had been installed. The version of the various libraries used had been included just in case some user face compatibility issue. As a normal practice, just install the latest version. It is not necessary to use the same version unless there is problem compiling the sketch.


<BR>

### Arduino Libraries used in ***espSnap*** sketch as follows:
###### Board Manager
- Arduino ESP32 Boards - 2.0.18

- esp32 by Espressif Systems - 3.0.4

- esp8266 by ESP8266 Community - 3.1.2

###### Library Manager
- floatToString by Ted Toal - 1.3.1

- DHT22 by dvarrel - 1.0.7

- OneWire by Jim Studt - 2.3.8

- Dallas Temperature by Miles Burton - 3.9.0

- Adafruit NeoPixel by Adafruit - 1.12.3

- LiquidCrystal_PCF8574 by Matthias Hertel - 2.2.0

- Adafruit GFX Library by Adafruit - 1.11.11

- Adafruit BusIO by Adafruit - 1.16.1

- Adafruit SSD1306 by Adafruit - 2.5.12

- ESP32Servo by Kevin Harrington - 3.0.5

