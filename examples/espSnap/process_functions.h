#ifndef I2C_LCD_OLED_H_ 
#define I2C_LCD_OLED_H_ 

#ifdef ESP8266
  // ESP8266
  // #define DEBUG_ESP_MDNS_ERR
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <ESP8266WebServer.h>
  // #include <time.h>
  // #include <PolledTimeout.h>
  #include <ESP8266mDNS.h>
#endif ESP8266

#ifdef ESP32
  // ESP32
  #include <WiFi.h>
  #include <ESPmDNS.h>
  #include <NetworkClient.h>
  #include <WebServer.h>
#endif ESP32


#include "notes_pitches.h"
#include "http_urls.h"
// #include <stdint.h>
#include <StreamString.h>
#include <floatToString.h> // need to install library by Ted Toal - v1.3.1 installed
#include <string.h>
/**************************************************/


// Tone 
int tone_process_cmd(String cmd_str);

#include "notes_pitches.h"
// Tune
int tune_process_update(); // need to add to be called in the main loop
int tune_process_cmd(String cmd_str);

// DHT22 library by dvarrel - v1.0.7
// https://github.com/dvarrel/DHT22
#include <DHT22.h>
// void 1_wire_process_dht22(String *cmd_str);

void DHT22_process_cmd(int DataPin, char * resp_buf);


// HC-SR04
float HCSR04_process_cmd(int trigPin, int echoPin);


// DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>

void ds18b20_process_cmd(int DataPin, char *respbuf);


// Adafruit NeoPixel library
#include <Adafruit_NeoPixel.h>
int neopixel_process_update(); // need to add to be called in the main loop
void neopixel_process_init(int pin, int count);

int neopixel_process_cmd(String cmd_str);


// I2C LCD 16x2 library for HD44780 controller
// https://github.com/johnrickman/LiquidCrystal_I2C
// #include <LiquidCrystal_I2C.h>


// LiquidCrystal_PCF8574 by mathertel
// https://github.com/mathertel/LiquidCrystal_PCF8574
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
// set the LCD number of columns and rows
// int lcdColumns;
// int lcdRows;
// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
// LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  
// LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  
#define I2C_LCD_COLUMNS 16
#define I2C_LCD_ROWS 2
#define I2C_LCD_ADDRESS 0x27
int i2c_lcd_process_cmd(String cmd_str);


// I2C OLED library for 128x32 and 128x64 0.96" SD1306
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
// On an ESP8266:           D2(SDA), D1(SCL)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

// 128x32 OLED SSD1306
// #define OLED_SCREEN_WIDTH 128 // OLED display width, in pixels
// #define OLED_SCREEN_HEIGHT 32 // OLED display height, in pixels
// #define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// 128x64 OLED SSD1306
#define OLED_SCREEN_WIDTH 128 // OLED display width, in pixels
#define OLED_SCREEN_HEIGHT 64 // OLED display height, in pixels
// #define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define OLED_SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// void handle_i2c_oled();
// void neopixel_process_init(int pin, int count);
int i2c_oled_process_cmd(String cmd_str);


// DIx2
// S1 
// S2
// DIx1 optional
// Push Button - just use digital input
void rotary_encoder_process_update();
void rotary_encoder_process_cmd(String cmd_str, char * resp_buf);

#endif I2C_LCD_OLED_H_
