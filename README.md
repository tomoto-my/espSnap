# _espSnap_

## A Http Server bridging Snap4Arduino and ESP32/ESP8266 Arduino boards

**_espSnap_** is a Http Server running on ESP32 or ESP8266 to serve as a bridge between **Snap4Arduino** and Arduino boards. It enables **Snap4Arduino** users to control electronic hardwares/modules connected to the Arduino board.

It follows the concept of [**Firmata Lbirary**](https://docs.arduino.cc/retired/hacking/software/FirmataLibrary/) and [**The Telemetrix Project**](https://github.com/MrYsLab/telemetrix) which allows communication between Arduino boards and host computer. However, this implementation **DO NOT** follow the [**Firmata Protocol**](https://github.com/firmata/protocol), or the approach taken by **The Telemetrix Project** using a Python client.

In this implementation, we used http via the ***url block*** ![**url**](assets/images/url_block_espsnap_local_h20.png) in **Snap4Arduino** as the integration method. With ESP32 or ESP8266 as the target Arduino boards we planned to use, **Wi-Fi** is the preferred communication method between PC running **Snap4Arduino** and the **_espSnap_** Arduino board.

[**espSnap_Library**](https://github.com/tomoto-my/espSnap/tree/main?tab=readme-ov-file#espSnap_Library-for-Snap4Arduino) contains **"Custom Blocks"** for **_espSnap_** is provided as a library for **Snap4Arduino**. Users can use the **espSnap_Library** **"Custom Blocks"** to interact with the **_espSnap_** Arduino board and modules transparently. This makes the **_espSnap_** function as an integrated part of **Snap4Arduino**. Just click on "File->Import..." in **Snap4Arduino** and select the file to import the **espSnap_Library**.

<BR>

---
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


| Additional Functions        | Arduino Parts           |
|:----------------------------|:------------------------|
| I2C OLED                    | I2C OLED SSD1306 128x64 |
| I2C LCD                     | I2C LCD 16x2            |
| Ultrasonic Distance Sensor  | HC-SR04                 |
| Temperature/Humidity Sensor | DTH22                   |
| Temperature Sensor          | DS18B20                 |
| NeoPixel                    | WS2812B                 |
| LDR Light Sensor            | KY-018                  |
| Rotary Encoder              | EC11 / KY-040           |
| Rain Sensor Module          | YL-83                   |



#### ESP32 board tested
 - Wemos R1 D32


#### ESP8266 board tested
 - NodeMCU 1.0 (ESP-12E Module)


Latest release [![espSnap release-v1.0.2](/assets/images/github-espSnap_release_1.0.2-blue.svg)](https://github.com/tomoto-my/espsnap/releases/latest/)


<BR>

---
## espSnap_Library for Snap4Arduino

**espSnap_Library** for **Snap4Arduino** is a library file with all **Custom Blocks** for **_espSnap_** and is available as in the file

[**espSnap_Library.xml**](espSnap_Custom_Blocks_Library/espSnap_Library.xml)

It contains all the **"Custom Blocks"** for all functions in **Basic Arduino Functions** and **Additional Functions** listed above, and can be imported into any new or existing **Snap4Arduino** project file.

Just start a new project file or open an existing project file from **Snap4Arduino** and go to **"Files->Import"** and select the library file provided. The **"Custom Blocks"** will be available for use.

For detailed explanations and examples on how to use the **espSnap_Library**, refer to the [**Companion Tutorials with Examples**](https://github.com/tomoto-my/espSnap/tree/main?tab=readme-ov-file#companion-tutorials-with-examples) below.


<BR>

---
## Companion Tutorials with Examples

**Companion Tutorials with Examples** is available at :

[**Nan Hwa Snap4Arduino Training**](https://github.com/tomoto-my/Nan-Hwa-Snap4Arduino-Training)

This companion tutorial contains explanations and instructions with examples.

It has 3 sections as follows :
- Section 1 on **Basic Introduction to Snap_!_** for **Snap4Arduino**
- Section 2 on **Basic Arduino Functions** of the **espSnap_Library** for **Snap4Arduino**
- Section 3 on **Additional Functions** of the **espSnap_Library** for **Snap4Arduino**

<BR>

---

# Installation of ***espSnap*** on ESP32/ESP8266 board

Follow the following steps to download and install ***espSnap*** on the ESP32/ESP8266 board of your choice

- Download the latest version of ***[espSnap_main.zip](https://github.com/tomoto-my/espSnap/archive/refs/heads/main.zip)***

- Extract it under your `Arduino/libraries` subdirectory
    - by default the name of the subdirectory will be "espSnap-main"
    - you may just leave it as "espSnap-main" or rename it to "espSnap"
    - **Arduino IDE** will ignore the "-main" at the end of the name

- Follow the usual steps of selecting the ESP32/ESP8266 board to program in **Arduino IDE**

- Use your **Arduino IDE** to open the **_espSnap_** Arduino Sketch from File->Examples->"Examples from Custom Libraries"->espSnap->**_espSnap_**

- **If you plan to use a local Wi-Fi Access Point, remember to change the SSID and PASSWORD.** Or, otherwise, the SSID and PASSWORD can be change any time via web browser.

- **The sketch needs bigger code space than the "Default" setting.** <BR>
    Hence need to change **Partition Scheme** setting as follow
    - **WEMOS D1 R32**
        - Partition Scheme: <BR>
        **"No OTA (Large APP)"** or <BR>
        **"Minimal SPIFFS (Large APPS with OTA)"**
    - **NodeMCU 1.0 (ESP-12E Module)**
        - Flash Size: <BR>
        **"4MB (FS:2MB OTA:~1019KB)"**

- Compile and Upload **_espSnap_** to the ESP32/ESP8266 Arduino board

<BR>

---
### Setup and connect to _espSnap_

***espSnap*** can be configured to connect to any local **Wi-Fi** Access Point, or on failure, will start up under **SoftAP mode** allowing up to 4 host computers to connect to it.

The default **SoftAP mode** (when it cannot connect to the local Wi-Fi AP)<BR>
- SSID : **espsnap_xxxxxx** <BR>
    where xxxxxx is the last 6 characters of MAC address<BR>
- PASSWORD : **test1234**<BR>

**mDNS** has been configured in **_espSnap_**, this allows the use of domain name in url instead of IP address. The default **mDNS ID** is "***espsnap.local***"<BR>


To test connection between host computer and ***espSnap***, use any web browser and type the following url in the address bar

**http://espsnap.local/**

A web page with **_espSnap_** basic information should show up confirming the connection between the PC and the **_espSnap_**. Users can proceed to **Snap4Arduino** and import the [**espSnap_Library.xml**](espSnap_Custom_Blocks_Library/espSnap_Library.xml) to use the functions provided by **_espSnap_** as listed below.

If the web browser cannot access the web page from **_espSnap_**, then please check the followings:
- power supply for **_espSnap_** had been connected and **"ON"**
    - the **_built-in led_** on Wemos D1 R32 or NodeMCU 1.0 (ESP-12E Module) should blink for a short period during power on
    - this blinking action indicates **_espSnap_** is trying to connect to the local AP which had been configured previously
- Wi-Fi connection for **_espSnap_**
    - check the status of the **_built-in led_** on **Wemos D1 R32** or **NodeMCU 1.0 (ESP-12E Module)**
        - remains permanently **"ON"**, then it is on **Soft-AP** mode
        - remains permanently **"OFF"**, then it is connected to the **local AP**
- Please make sure the PC is connected to the same AP as the **_espSnap_**
- Please check whether there is any **firewall/anti-virus** which **blocks access** to the **"http://espsnap.local/"**.
    - The **_espSnap_** should be within the local area network.
    - You will need to either
        - disable the firewall, which may be a security issue if the local AP is connected to internet
        - from the firewall settings, allow access to local area network only, or the particular IP address only
    - retry until connected from web browser to **_espSnap_** had been tested ok.

mDNS, SSID and PASSWORD can be change via web browser from **http://espsnap.local/config**



---
### Arduino Libraries used in ***espSnap*** sketch as follows:

If some libraries required are missing during compilation, please make sure the list of libraries as listed below had been installed. The version of the various libraries used had been included just in case some user face compatibility issue. As a normal practice, just install the latest version. It is not necessary to use the same version unless there is problem compiling the sketch.

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

