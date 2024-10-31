# espSnap

## A Http Server bridging Snap4Arduino and ESP32/ESP8266 Arduino boards

**_espSnap_** is a Http Server running on ESP32 or ESP8266 to serve as a bridge between **Snap4Arduino** and Arduino boards. It enables **Snap4Arduino** users to control electronic hardwares/modules connected to the Arduino board.

It follows the concept of [**Firmata Lbirary**](https://docs.arduino.cc/retired/hacking/software/FirmataLibrary/) which allows communication between Arduino boards and host computer. However, this implementation **DO NOT** follow the [**Firmata Protocol**](https://github.com/firmata/protocol).

In this implementation, we used http via the **_url block_** ![**url**](assets/images/url_block_espsnap_local_h20.png) in **Snap4Arduino** as the integration method. Hence, **Wi-Fi** is chosen as the communication means, and ESP32 or ESP8266 are the preferred Arduino boards we had developed and test it for.

We provide **"Custom Blocks"** in **Snap4Arduino** so that **Snap4Arduino** users will feel the Arduino board as an integrated part of it, and focus on using it instead of learn how to use it.

**_espSnap_** can be configured to connect to any local **Wi-Fi** Access Point, or on failure, will start up under SoftAP mode allowing up to 4 host computers to connect to it.

The default mDNS is "**_espsnap.local_**"<BR>
and the default Soft AP<BR>
- SSID : **espsnap_xxxxxx** <BR>
    where xxxxxx is the last 6 characters of MAC address<BR>
- PASSWORD : **test1234**<BR>

To test connection between host computer and espSnap, use any web browser and type the following url in the address bar<BR>
**http://espsnap.local/**

The functionality implemented by **_espSnap_** are as follows


| Basic Arduino Functions  |
|:------------------------:|
| Digital Input            |
| Digital Output           |
| Analog Input             |
| Analog Output            |
|   - DAC[^1]             |
|   - PWM                  |
|   - Servo                |
|   - Tone                 |

[^1] : Analog Output - DAC is only available on ESP32


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
## Companion Tutorials with Examples Available at

**Companion Tutorials and Examples : [Nan Hwa Snap4Arduino Training](https://github.com/tomoto-my/nanhwasnap4arduinotraining/)**

This companion tutorial has 1 section as Basic Introduction to **_Snap_** and 2 sections with examples on using the **"Custom Blocks"** of **_espSnap_** in **Snap4Arduino**
<BR>
<BR>

---

### Arduino Libraries used in espSnap sketch as follows:
Board Manager
- Arduino ESP32 Boards - 2.0.18
- esp32 by Espressif Systems - 3.0.4
- esp8266 by ESP8266 Community - 3.1.2

Library Manager
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



