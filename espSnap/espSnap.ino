/* espSnap HttpServer for Snap4Arduino
  Copyright (c) 2024, Tomoto Sdn Bhd
  - compatible with ESP32
  - tested on ESP32 
    - Wemos D1 R32
  - tested on ESP8266
    - NodeMCU 1.0(ESP-12 Module)
*/
// #define SERIAL_VERBOSE

#define VERSION "V1.0.0"

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
// // ESP32
// ESP32 GPIOs
// Digital Output - 0,1,2,3,4,5,12,13,14,15,16,17,18,19,21,22,23,25,26,27,32,33 
// Digital Input - 0,1,2,3,4,5,12,13,14,15,16,17,18,19,21,22,23,25,26,27,32,33,34,35,36,39
// Analog Input - GPIO34, GPIO35, GPIO36, GPIO39 + GPIO15, GPIO32, GPIO33
// ESP32 : ADC1 pins available are GPIO34, GPIO35, GPIO36, GPIO39 (GPIO32 and GPIO33 available without pins)
// ESP32 : ADC2 does not work when wifi is working
// Analog Output - GPIO25 / GPIO26
// PWM - 
// I2C - 
// SCL : GPIO22
// SDA : GPIO21
// SPI - 
// MOSI : GPIO23
// MISO : GPIO19
// CLK  : GPIO18
// CS   : GPIO5


  #include <WiFi.h>
  #include <WiFiAP.h>
  #include <ESPmDNS.h>
  #include <NetworkClient.h>
  #include <WebServer.h>
#endif ESP32


#include <StreamString.h>
#include <FS.h>
#include <LittleFS.h>
#include <floatToString.h> // need to install library by Ted Toal - v1.3.1 installed

#include <uri/UriBraces.h> 
#include <uri/UriRegex.h>


#include "LittleFS_save_config.h"
#include "http_urls.h"


#include "process_functions.h"

#ifdef ESP8266
  #include "Servo.h"
#endif ESP8266
#ifdef ESP32
  #include "ESP32Servo.h" // ESP32Servo by Kevin Harrington, John K. Bennett
#endif ESP32
Servo myservo; // create servo object to control a servo




// #define ALL_AHPHANUMERIC_SPECIAL "[a-z0-9A-Z ~`!@#$%^&*()-_+={[\]}\\|;:'\",<.>/?z]"
#define ALL_AHPHANUMERIC_SPECIAL         "[a-z0-9A-Z ~!$%&*()-_+={[}|;:,.'/]"
//#define OLED_ALL_AHPHANUMERIC_SPECIAL    "[a-z0-9A-Z ~!$%&*()-_+={[}|;:,.'/]"
#define OLED_ALL_AHPHANUMERIC_SPECIAL    "[a-z0-9A-Z ~!@$%&*()-_+={[}|;:,.'/]"
// #define OLED_ALL_AHPHANUMERIC_SPECIAL "[a-z0-9A-Z ~`!@$^&*()-_={[]}\\|;:'\",<.>/z]"

#define TIMEZONE_OFFSET 8        // CET
#define DST_OFFSET 1             // CEST
#define UPDATE_CYCLE (1 * 1000)  // every second

// #ifndef APSSID
#define APSSID "espsnap_"
// #define APPSK "password"
#define APPSK "12341234" // password MUST BE 8 characters or more
// #endif

#define mDNSID "espsnap"

String espsnap_version = VERSION;

// #ifndef STASSID
// #define STASSID ""
// #define STAPSK ""
// #endif

const char *ap_default_ssid = APSSID;  ///< Default AP SSID.
const char *ap_default_psk = APPSK;    ///< Default AP PSK.
const char *mdns_default_id = mDNSID;    ///< Default mDNS ID

// const char *ssid = STASSID;
// const char *password = STAPSK;

String station_ssid = "";
String station_psk = "";
String mdns_id = mdns_default_id;

// MDNSResponder::hMDNSService hMDNSService = 0;  // The handle of the clock service in the MDNS responder

#ifdef ESP8266
  ESP8266WebServer server(80);
#endif ESP8266

#ifdef ESP32
  // NetworkServer server(80);
  WebServer server(80);
#endif ESP32


// NodeMCU BUILTIN LED pin is 2
// const int led = 13;
const int led = 2; //

unsigned long last_announce = 0;
const unsigned long announce_interval = 120000; // 120 seconds

boolean softAP = false;


//
// flag & buffer for serial commands
//
String Serial_inputString = "";      // a String to hold incoming data
bool Serial_stringComplete = false;  // whether the string is complete
int Serial_serial_state = 0;
bool Serial_flag_update_wifi_settings = false;
bool Serial_flag_reconnect_wifi = false;
String Serial_temp_ssid = "";
String Serial_temp_pwd = "";
String Serial_temp_mdns_id = "";
//

// programmer misses printf...
void debug_print(const char *format, ...)
{
#ifdef SERIAL_VERBOSE
  char buffer[256];  // or smaller or static &c.
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);
  Serial.print(buffer);
#endif SERIAL_VERBOSE
}

void handleRoot() {
  // digitalWrite(led, 1);
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  StreamString temp;
  temp.reserve(500);  
  temp.printf("\
<!DOCTYPE html>\r\n\
<html><style>table, th, td {border:1px solid black;}</style>\r\n\
  <body>\r\n\
    <table style=\"width:70%\"><tr><th><h1>espSnap</h1>\
    <h3>HttpServer for Snap4Arduino</h3><h4>%s<BR>by tomoto.my<BR></h4>\
    <h2>access use <a href=\"http://%s.local/\">http://%s.local/</a> from Snap4Arduino<BR></h2></th></tr>\r\n\
    <tr><th><h3>use <a href=\"http://%s.local/config\">http://%s.local/config</a> to change settings</h3></th></tr>\r\n\
    </table>\r\n\
    <table style=\"width:70%\"><tr><th colspan=\"4\">Basic Arduino Functions</th></tr>\r\n\
    <tr><td colspan=\"3\">Digital Input</td></tr>\r\n\
    <tr><td colspan=\"3\">Digital Output</td></tr>\r\n\
    <tr><td colspan=\"3\">Analog Input</td></tr>\r\n\
    <tr><td colspan=\"2\">Analog Output :</td><td>DAC</td></tr>\r\n\
    <tr><td colspan=\"2\"></td><td>PWM</td></tr>\r\n\
    <tr><td colspan=\"2\"></td><td>Servo</td></tr>\r\n\
    <tr><td colspan=\"2\"></td><td>Tone</td></tr>\r\n\
    <tr><th colspan=\"4\">Additional Functions Available :</th></tr>\r\n\
    <tr><td colspan=\"3\">I2C OLED</td><td>I2C OLED SSD1306</td></tr>\r\n\
    <tr><td colspan=\"3\">I2C LCD</td><td>I2C LCD16x2-I2C</td></tr>\r\n\
    <tr><td colspan=\"3\">Ultrasonic Distance Sensor</td><td>HC-SR04</td></tr>\r\n\
    <tr><td colspan=\"3\">Temperature/Humidity Sensor</td><td>DHT22</td></tr>\r\n\
    <tr><td colspan=\"3\">Temperature Sensor</td><td>DS18B20</td></tr>\r\n\
    <tr><td colspan=\"3\">NeoPixel</td><td>WS2812B</td></tr>\r\n\
    <tr><td colspan=\"3\">LDR Light Sensor</td><td>KY-018</td></tr>\r\n\
    <tr><td colspan=\"3\">Rotary Encoder</td><td>EC11 / KY-040</td></tr>\r\n\
    <tr><td colspan=\"3\">Rain Sensor Module</td><td></td></tr>\r\n\
    </table>\r\n\
    <table style=\"width:70%\"><tr><th><h3>espSnap</h3>\
    <h4>HttpServer for Snap4Arduino<BR>source code for ESP32 and ESP8266 for Arduino</h4>\r\n\
    <h4><a href=\"https://github.com/tomoto-my/espsnap/\">https://github.com/tomoto-my/espsnap/</a></h4>\r\n\
    </table>\r\n\
    <p>Uptime: %02d:%02d:%02d</p>\r\n\
  </body>\r\n\
</html>",
     espsnap_version, mdns_id, mdns_id, mdns_id, mdns_id, hr, min % 60, sec % 60);
  server.send(200, "text/html", temp.c_str());
  #ifdef SERIAL_VERBOSE
    Serial.println("/handleRroot():");
  #endif SERIAL_VERBOSE
  // digitalWrite(led, 0);
}

// retrieve IP address of espSnap
//http://espsnap.local/getip
void handleGetIP() {
  String ip_str = WiFi.localIP().toString();
  Serial.println(ip_str);
  String resp = "OK\nIP address\n" + ip_str + "\n";
  server.send(200, "text/plain", resp);
  #ifdef SERIAL_VERBOSE
    Serial.println(resp);
  #endif SERIAL_VERBOSE
}

// pinmode - INPUT/OUTPUT/INPUT_PULLUP
// 0 : INPUT
// 1 : OUTPUT
// 2 : INPUT_PULLUP
//http://espsnap.local/pinmode/2/1 - pin + pinmode
void handlePinMode() {
  String pin_str = server.pathArg(0);
  String mode_str = server.pathArg(1);
  int setPin = pin_str.toInt();
  int setMode = mode_str.toInt();
  if ((setMode < 0) || (setMode > 2)) setMode = 0;
  pinMode(setPin, setMode); // set pinmode
  String resp = "OK\nPinMode\n" + pin_str + "\n" + mode_str;
  server.send(200, "text/plain", resp);
  #ifdef SERIAL_VERBOSE
    Serial.println(resp);
  #endif SERIAL_VERBOSE
}

//http://espsnap.local/dout/13/1 - digital input + pin + ON/OFF
void handleDigitalOutput() {
  String pin_str = server.pathArg(0);
  String value_str = server.pathArg(1);
  int outputPin = pin_str.toInt();
  int outputValue = 0;
  if (value_str != "0") outputValue = 1;
  pinMode(outputPin, OUTPUT); // added pinmode setting here to make it easy for users 
  digitalWrite(outputPin, outputValue);
  String resp = "OK\nDO\n" + pin_str + "\n" + value_str;
  server.send(200, "text/plain", resp);
  #ifdef SERIAL_VERBOSE
    Serial.println(resp);
  #endif SERIAL_VERBOSE
}

//http://espsnap.local/din/13 - digital input + pin
void handleDigitalInput() {
  String pin_str = server.pathArg(0);
  int digitalPin = pin_str.toInt();
  pinMode(digitalPin, INPUT_PULLUP); // added pinmode setting here to make it easy for users 
  int digitalValue = digitalRead(digitalPin);
  String resp = "OK\nDI\n" + pin_str + "\n" + digitalValue;
  server.send(200, "text/plain", resp);
  #ifdef SERIAL_VERBOSE
    Serial.println(resp);
  #endif SERIAL_VERBOSE
}

//http://espsnap.local/ain/0 - analog input + pin
// ESP32 : ADC2 does not work when wifi is working
// ESP32 : ADC1 pins available are GPIO34, GPIO35, GPIO36, GPIO39 (GPIO32 and GPIO33 available without pins)
// ESP32 ADC is 12-bits => return value 0 to 4095
void handleAnalogInput() {
  String pin_str = server.pathArg(0);
  // String value = server.pathArg(1);
  int analogPin = pin_str.toInt();
  // pinMode(analogPin, INPUT);
  int analogValue = analogRead(analogPin);
  // server.send(200, "text/plain", "OK-DI:" + pin + ":" + value);
  String resp = "OK\nAI\n" + pin_str + "\n" + analogValue;
  server.send(200, "text/plain", resp);
  #ifdef SERIAL_VERBOSE
    Serial.println(resp);
  #endif SERIAL_VERBOSE
}

//http://espsnap.local/aout/0 - analog output + pin + value
// ESP32 has 2 DAC available are GPIO25 and GPIO26
// ESP32 DAC has 3 mode 
// ONLY implemented Direct Voltage Output mode here 
// ESP32 DAC is 8-bits => gives output voltage 0 to ~3.3V
void handleAnalogOutput() {
  String pin_str = server.pathArg(0);
  String value_str = server.pathArg(1);
  int analogPin = pin_str.toInt();
  int analogValue = value_str.toInt();
  #ifdef ESP8266
    #warning ESP8266 do not have DAC
    Serial.println("ESP8266 do not have DAC");
  #endif ESP8266
  #ifdef ESP32
    // dac set output value
    dacWrite(analogPin, analogValue);
  #endif ESP32
  String resp = "OK\nAO\n" + pin_str + "\n" + analogValue;
  server.send(200, "text/plain", resp);
  #ifdef SERIAL_VERBOSE
    Serial.println(resp);
  #endif SERIAL_VERBOSE
}

//http://espsnap.local/pwm/13/0-255 - pwm + pin + value (0-255)
void handlePWM() {
  String pin_str = server.pathArg(0);
  String value_str = server.pathArg(1);
  int outputPin = pin_str.toInt();
  int outputValue = value_str.toInt();
  if (outputValue < 0) outputValue = 0;
  if (outputValue > 255) outputValue = 255;
  #ifdef SERIAL_VERBOSE
    Serial.println("value=" + String(outputValue));
  #endif SERIAL_VERBOSE
  pinMode(outputPin, OUTPUT); // added pinmode setting here to make it easy for users 
  analogWrite(outputPin, outputValue);
  String resp = "OK\nPWM\n" + pin_str + "\n" + value_str;
  server.send(200, "text/plain", resp);
  #ifdef SERIAL_VERBOSE
    Serial.println(resp);
  #endif SERIAL_VERBOSE
}

// 0-180 degree Servo Motor
//http://espsnap.local/servo/13/value - servo + pin + value (0-180)
void handleServo() {
  String pin_str = server.pathArg(0);
  String value_str = server.pathArg(1);
  int outputPin = pin_str.toInt();
  int outputValue = value_str.toInt();
  if (outputValue < 0) outputValue = 0;
  if (outputValue > 180) outputValue = 180;
  #ifdef SERIAL_VERBOSE
    Serial.println("pin=" + String(outputPin));
    Serial.println("value=" + String(outputValue));
  #endif SERIAL_VERBOSE
  // pinMode(outputPin, OUTPUT); // added pinmode setting here to make it easy for users 
  myservo.attach(outputPin);
  myservo.write(outputValue);
  String resp = "OK\nServo\n" + pin_str + "\n" + value_str;
  server.send(200, "text/plain", resp);
  #ifdef SERIAL_VERBOSE
    Serial.println(resp);
  #endif SERIAL_VERBOSE
}

//http://espsnap.local/hcsr04/12/14 - trig_pin + echo_pin
void handleHCSR04() {
  String trigpin_str = server.pathArg(0);
  String echopin_str = server.pathArg(1);
  int trigPin = trigpin_str.toInt();
  int echoPin = echopin_str.toInt();
    // Serial.println("trig_pin=" + String(trigPin));
    // Serial.println("echo_pin=" + String(echoPin));

  float distancemm;
  distancemm = HCSR04_process_cmd(trigPin, echoPin);

  char distancemm_buf[15];
  floatToString(distancemm, distancemm_buf, sizeof(distancemm_buf), 4);
  // String resp = "OK\nHC_SR04\n" + trigpin_str + "\n" + echopin_str;
  String distancemm_str = distancemm_buf;
  // String resp = "OK\nHC_SR04\n" + trigpin_str + "/" + echopin_str + "\n" + distancemm_str + "\n(mm)\n";
  String resp = "OK\nHC_SR04\n" + distancemm_str + "\nmm\n";
  server.send(200, "text/plain", resp);
  #ifdef SERIAL_VERBOSE
    Serial.println(resp);
  #endif SERIAL_VERBOSE
}

//http://espsnap.local/dht22/0 - dht22 + pin
void handleDHT22() {
  String pin_str = server.pathArg(0);
  int DataPin = pin_str.toInt();
  // Serial.println("pin=" + String(DataPin));

  char resp_buf[100];
  DHT22_process_cmd(DataPin, resp_buf);

  String resp = String(resp_buf);
  // String resp = "OK\nDHT22\n" + resultstr;
  server.send(200, "text/plain", resp);
  #ifdef SERIAL_VERBOSE
    Serial.println(resp);
  #endif SERIAL_VERBOSE
}


//http://espsnap.local/ds18b20/13 - ds18b20 + pin
void handleDS18B20() {
  String pin_str = server.pathArg(0);
  int DataPin = pin_str.toInt();
  // Serial.println("pin=" + String(DataPin));

  char resp_buf[100];
  // Serial.println((unsigned long) resp_buf);
  ds18b20_process_cmd(DataPin, resp_buf);

  // Serial.println("resp_buf:");
  // Serial.println(resp_buf);

  String resp = String(resp_buf);
  server.send(200, "text/plain", resp);
  #ifdef SERIAL_VERBOSE
    Serial.println(resp);
  #endif SERIAL_VERBOSE
}

//http://espsnap.local/neopixel/nn,xxxxxx - number of neopixel + (3*nn)bytes hex string of GRB value
void handleNeoPixel() {
  // char cmd_str[2001];
  // server.pathArg(0).toCharArray(cmd_str, 2000);
  String cmd_str = server.pathArg(0);
  // log_d("neopixel_cmd_str=%p:%s", cmd_str, cmd_str);

  int err_num = neopixel_process_cmd(cmd_str);
  String resp;
  if (err_num == 0) {
    resp = "OK\nNeoPixel\n0\nOK\n";
    server.send(200, "text/plain", resp);
  }
  else {
    resp = "NAK\nNeoPixel\n" + String(err_num) + "\nInvalid Command\n";
    server.send(200, "text/plain", resp);
  }

  // Serial.println(resp);
}

//http://espsnap.local/tone/13,C4,4 - tone + pin + note + duration
void handleTone() {
  String cmd_str = server.pathArg(0);
  // log_d("tone_cmd:%s",cmd_str);

  int err_num = tone_process_cmd(cmd_str);

  String resp = "OK\ntone";
  server.send(200, "text/plain", resp);
  // log_d("tone:%s",resp);
}

//plays a tune
//http://espsnap.local/tune/13,[C4,250,....] - tune + pin + [note + duration + ....]
void handleTune() {
  String cmd_str = server.pathArg(0);
  // log_d("tune_cmd:%s",cmd_str);

  int err_num = tune_process_cmd(cmd_str);

  String resp = "OK\ntune";
  server.send(200, "text/plain", resp);
  // log_d("tune:%s",resp);
}

// ESP32 Wemos D1 R32 default pin is
// GPIO22 : SCL
// GPIO21 : SDA
// ESP8266 I2C default pin is
// D1 : GPIO5 : SCL
// D2 : GPIO4 : SDA
//http://espsnap.local/i2c_lcd/ - i2c_lcd
void handle_i2c_lcd() {
  char cmd_str[101];
  server.pathArg(0).toCharArray(cmd_str, 100);
  // log_d("i2c_lcd_cmd_str=%p:%s", cmd_str, cmd_str);
  // Serial.println(cmd_str);

  int result = i2c_lcd_process_cmd(cmd_str);

  String resp = "OK\ni2c_lcd\n" + String(result) + "\n" + cmd_str + "\n";
  server.send(200, "text/plain", resp);
  #ifdef SERIAL_VERBOSE
    Serial.println(resp);
  #endif SERIAL_VERBOSE
}


// ESP32 Wemos D1 R32 default pin is
// GPIO22 : SCL
// GPIO21 : SDA
// ESP8266 I2C default pin is
// D1 : GPIO5 : SCL
// D2 : GPIO4 : SDA
// character 
//http://espsnap.local/i2c_oled/ - i2c_oled
void handle_i2c_oled() {
  String cmd_str = server.pathArg(0);
  // char tempbuf[100];
  // snprintf(tempbuf, sizeof(tempbuf), "cmd_str=%s\n", cmd_str.c_str());
  // Serial.print(tempbuf);

  // Serial.println((long) &cmd_str);

  int result = i2c_oled_process_cmd(cmd_str);

  String resp = "OK\ni2c_oled\n" + String(result) + "\n" + cmd_str + "\n";
  server.send(200, "text/plain", resp);
  #ifdef SERIAL_VERBOSE
    Serial.println(resp);
  #endif SERIAL_VERBOSE
}

// rotary encoder - EC11 - DIx2 + DIx1 optiontal
void handle_rotaryencoder() {
  String cmd_str = server.pathArg(0);

  char resp_buf[100];
  rotary_encoder_process_cmd(cmd_str, resp_buf);

  String resp = String(resp_buf);
  server.send(200, "text/plain", resp);
  #ifdef SERIAL_VERBOSE
    Serial.println(resp);
  #endif SERIAL_VERBOSE
}


void handleNotFound() {
  // Serial.println("handleNotFound()");
  // digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) { message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; }

  server.send(404, "text/plain", message);
  // digitalWrite(led, 0);
}


void handleConfigForm() {
  // Serial.println("handleConfig()");
const String postForm1 = "<html>\
  <head>\
    <title>espSnap WiFi Settings</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      th { text-align: left; }\
    </style>\
  </head>\
  <body>\
    <h1>espSnap WiFi Settings</h1><br>\
    <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/postform/\">\
      <table>\
        <tr><th>WiFi SSID : </th><td><input type=\"text\" name=\"ssid\" value=\"";
const String postForm2 = "\"></td></tr>\
        <tr><th>WiFi Password : </th><td><input type=\"text\" name=\"password\" value=\"";
const String postForm3 = "\"></td></tr>\
        <tr><th>mDNS ID : </th><td><input type=\"text\" name=\"mdnsid\" value=\"";
const String postForm4 = "\"></td></tr>\
      </table>\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
  </body>\
</html>";

  String postForm;
  postForm.reserve(2600);

  postForm += postForm1;
  postForm += station_ssid;
  postForm += postForm2;
  // postForm += station_psk;
  postForm += postForm3;
  postForm += mdns_id;
  postForm += postForm4;
  server.send(200, "text/html", postForm);
}


void handleConfig() {
  // String station_ssid = "";
  // String station_psk = "";

  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    digitalWrite(led, 0);
  } else {
    digitalWrite(led, 1);
    String message = "POST form was:\n";
    // for (uint8_t i = 0; i < server.args(); i++) { message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; }
    station_ssid  = server.arg(0);
    station_psk = server.arg(1);
    mdns_id = server.arg(2);
    message = server.argName(0) + " : " + station_ssid + "\n";
    message += server.argName(1) + " : " + station_psk + "\n";
    message += server.argName(2) + " : " + mdns_id + "\n";
    server.send(200, "text/plain", message);
    Serial.print("New SSID : ");
    Serial.println(station_ssid);
    Serial.print("New Password : ");
    Serial.println(station_psk);
    Serial.print("New mDNS ID : ");
    Serial.println(mdns_id);
    // delete current wifi settings in "cl_conf.txt"
    deleteConfig();
    delay(100);
    saveConfig(&station_ssid, &station_psk, &mdns_id); // create and save wifi settings to "/cl_conf.txt"

    delay(1000);
    ESP.restart(); // reboot to start with new wifi settings
  }
}

void handleConfigPlain() {
  // Serial.println("handleConfig()");
const String postForms = "<html>\
  <head>\
    <title>espSnap Http Server POST handling</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>POST plain text to /postplain/</h1><br>\
    <form method=\"post\" enctype=\"text/plain\" action=\"/postplain/\">\
      <input type=\"text\" name=\"test\" value=\'{\"hello\": \"world\", \"trash\": \"trasssss\"}\'><br>\
      <input type=\"text\" name=\"test2\" value=\'{\"hello\": \"world\", \"yeere\": \"more\"}\'><br>\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
  </html>";

  server.send(200, "text/html", postForms);
}

void handlePlain() {
  if (server.method() != HTTP_POST) {
    digitalWrite(led, 1);
    server.send(405, "text/plain", "Method Not Allowed");
    digitalWrite(led, 0);
  } else {
    digitalWrite(led, 1);
    String message = "POST form was:\n";
    server.send(200, "text/plain", "POST body was:\n" + server.arg("plain"));
    digitalWrite(led, 0);
  }
}

void drawGraph() {
  String out;
  out.reserve(2600);
  char temp[70];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x += 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}



String macToString(const unsigned char* mac) {
  char buf[20];
  snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}

//////
// ESP8266 only
#ifdef ESP8266
  WiFiEventHandler stationConnectedHandler;
  void onStationConnected(const WiFiEventSoftAPModeStationConnected& evt) {
    Serial.print("Station connected: ");
    Serial.println(macToString(evt.mac));

    //////
    // only for ESP8266
    #ifdef ESP8266
      MDNS.announce(); // announce mDNS
    #endif ESP8266
    //////
  }
#endif ESP8266
// ESP32 only
#ifdef ESP32
  void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
    Serial.println("Connected to AP successfully!");
  }

  void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

      if (MDNS.begin(mdns_id)) { Serial.println("MDNS responder started"); }
      Serial.print("MDNS : ");
      Serial.println(mdns_id);
      // Add service to MDNS-SD
      MDNS.addService("http", "tcp", 80);
  }

  void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
    Serial.println("Disconnected from WiFi access point");
    Serial.print("WiFi lost connection. Reason: ");
    Serial.println(info.wifi_sta_disconnected.reason);
    Serial.println("Trying to Reconnect");
    WiFi.begin(station_ssid, station_psk);
  }
#endif ESP32
//////

//
//
//
void setup(void) {
  Serial.begin(115200);

  // reserve 200 bytes for the Serial_inputString:
  Serial_inputString.reserve(200);

  while (!Serial)
    {
    }


  delay(2000);

  Serial.println("\r\n");
  // Serial.println(ESP.getChipId(), HEX);
  String chipid_str = "";
  #ifdef ESP8266
    chipid_str = String(ESP.getChipId(), HEX);
    Serial.print("Chip ID: 0x");
    Serial.println(chipid_str);
  #endif
  #ifdef ESP32
    uint32_t chipId = 0;

    for (int i = 0; i < 17; i = i + 8) {
      chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }

    Serial.printf("ESP32 Chip model = %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
    Serial.printf("This chip has %d cores\n", ESP.getChipCores());
    Serial.print("Chip ID: ");
    chipid_str = String(chipId, HEX);
    Serial.println(chipid_str);
  #endif ESP32

  // String ap_ssid = ssid;
  #ifdef SERIAL_VERBOSE
    Serial.println("This is actually BSSID instead of MAC address");
    Serial.print("WiFi MAC address: ");
    Serial.println(WiFi.macAddress());
  #endif SERIAL_VERBOSE

  // Initialize file system.
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount file system");
    Serial.println("Format LittleFS now...");
    // assume not yet formatted - format LittleFS

    #ifdef ESP8266
    if (!LittleFS.begin()) {
    #endif ESP8266
    #ifdef ESP32
    if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
    #endif ESP32
      Serial.println("LittleFS FORMAT Failed");
      return;
    }
    Serial.println("LittleFS formatted");
    // return;
  }

  // manually delete wifi settings to "cl_conf.txt"
  // deleteConfig();

  // Load wifi connection information.
  Serial.println("try to loadConfig");
  if (!loadConfig(&station_ssid, &station_psk, &mdns_id)) {
    // no config file
    // station_ssid = STASSID;
    // station_psk = STAPSK;
    // saveConfig(&station_ssid, &station_psk); // create and save wifi settings to "/cl_conf.txt"

    Serial.println("No WiFi connection information available.");
    // turn on soft AP
    softAP = true;
  }
  else {
    // config settings loaded
    // WiFi.begin();
    #ifdef SERIAL_VERBOSE
      Serial.print("WiFi SSID: ");
      Serial.println(station_ssid.c_str());
      Serial.print("WiFi Password: ");
      Serial.println(station_psk.c_str());
      Serial.print("mDNS ID: ");
      Serial.println(mdns_id.c_str());
    #endif SERIAL_VERBOSE

    WiFi.setAutoReconnect(true);
    WiFi.mode(WIFI_STA);
    // delay(1000);
    WiFi.begin(station_ssid.c_str(), station_psk.c_str());

    // ... Pritn new SSID
    Serial.print("new SSID: ");
    Serial.println(WiFi.SSID());

    // ... Uncomment this for debugging output.
    WiFi.printDiag(Serial);


    Serial.println("Wait for WiFi connection...");

    // ... Give ESP 10 seconds to connect to station.
    unsigned long startTime = millis();
    int led_toggle = 0;
    pinMode(2, OUTPUT);
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 30000) {
      Serial.write('.');
      // Serial.print(WiFi.status());
      digitalWrite(2,led_toggle);
      led_toggle = (led_toggle) ? 0 : 1;
      delay(500);
    }
    Serial.println();

    // Check connection
    if (WiFi.status() == WL_CONNECTED) {
      // ... print IP Address
      Serial.println("Connected to ");
      Serial.print("WiFi SSID: ");
      Serial.println(station_ssid);
      // Serial.println(WiFi.SSID());
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      // add MDSN service
      // if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }
      if (MDNS.begin(mdns_id)) { Serial.println("MDNS responder started"); }
      Serial.print("MDNS : ");
      Serial.println(mdns_id);
      // Add service to MDNS-SD
      MDNS.addService("http", "tcp", 80);
    }
    else {
      softAP = true;
      digitalWrite(2,1); // turn on builtin LED to indicate it is SoftAP mode
    }
  }


  if(softAP) {
    // Serial.println("Configuring access point...");
    station_ssid = ap_default_ssid;
    // ap_ssid += String(ESP.getChipId(), HEX);
    station_ssid += chipid_str;
    station_psk = ap_default_psk;
    WiFi.setAutoReconnect(false);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(station_ssid, station_psk);

    IPAddress myIP = WiFi.softAPIP();
    Serial.println("SoftAP mode");
    Serial.print("WiFi AP: ");
    Serial.println(station_ssid);
    Serial.print("WiFi AP password: ");
    Serial.println(station_psk);
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    // add MDSN service
    // use default mdns id
    mdns_id = mdns_default_id;
    if (MDNS.begin(mdns_id)) { Serial.println("MDNS responder started"); }
    Serial.print("MDNS : ");
    Serial.println(mdns_id);
    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
  }



    // Register event handlers.
    // Callback functions will be called as long as these handler objects exist.
    // Call "onStationConnected" each time a station connects
    //////
    // ESP8266 only
    #ifdef ESP8266
      stationConnectedHandler = WiFi.onSoftAPModeStationConnected(&onStationConnected);
    #endif ESP8266
    // ESP32 only
    #ifdef ESP32
      if(!softAP) {
        WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
        WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
        WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
      }
    #endif ESP32
    //////

    // String url_tone_regex = String("^/tone/(") + String(ALL_AHPHANUMERIC_SPECIAL) + String("+)$");
    // String url_i2c_lcd_regex = String("^/i2c_lcd/(") + String(ALL_AHPHANUMERIC_SPECIAL) + String("+)$");
    // String url_i2c_oled_regex = String("^/i2c_oled/(") + String(OLED_ALL_AHPHANUMERIC_SPECIAL) + String("+)$");
    // String url_neopixel_regex = String("^/neopixel/(") + String(OLED_ALL_AHPHANUMERIC_SPECIAL) + String("+)$");


    // add enable CORS header
    server.enableCORS(true);

    // add http urls
    server.on("/", handleRoot);
    server.on("/getip", handleGetIP);
    server.on(UriBraces("/pinmode/{}/{}"), handlePinMode);
    server.on(UriBraces("/dout/{}/{}"), handleDigitalOutput);
    server.on(UriBraces("/din/{}"), handleDigitalInput);
    server.on(UriBraces("/ain/{}"), handleAnalogInput);
    server.on(UriBraces("/aout/{}/{}"), handleAnalogOutput);
    server.on(UriBraces("/pwm/{}/{}"), handlePWM);
    server.on(UriBraces("/servo/{}/{}"), handleServo);
    server.on(UriBraces("/tone/{}"), handleTone); // DOx1
    server.on(UriBraces("/tune/{}"), handleTune); // DOx1
    server.on(UriBraces("/hcsr04/{}/{}"), handleHCSR04); // DOx1+DIx1
    server.on(UriBraces("/dht22/{}"), handleDHT22); // 1-wire
    server.on(UriBraces("/ds18b20/{}"), handleDS18B20); // 1-wire
    // server.on(UriRegex(url_tone_regex), handleTone); // DOx1
    // server.on(UriRegex(url_i2c_lcd_regex), handle_i2c_lcd); // I2C
    // server.on(UriRegex(url_i2c_oled_regex), handle_i2c_oled); // I2C
    // server.on(UriRegex(url_neopixel_regex), handleNeoPixel); // DOx1
    server.on(UriBraces("/neopixel/{}"), handleNeoPixel); // DOx1
    server.on(UriBraces("/i2c_lcd/{}"), handle_i2c_lcd); // I2C
    server.on(UriBraces("/i2c_oled/{}"), handle_i2c_oled); // I2C
    server.on(UriBraces("/rotaryencoder/{}"), handle_rotaryencoder); // DIx2 + DIx1
    server.on("/test.svg", drawGraph);
    server.on("/inline", []() {
      server.send(200, "text/plain", "this works as well");
    });
    server.on("/version", []() {
      server.send(200, "text/plain", espsnap_version);
    });
    server.on(F("/hi"), []() {
      server.send(200, "text/plain", "Welcome to espSnap");
    });

    // // e.g. http://espsnap.local/users/53
    // server.on(UriBraces("/users/{}"), []() {
    //   String user = server.pathArg(0);
    //   server.send(200, "text/plain", "User: '" + user + "'");
    //   Serial.print("/users/ : ");
    //   Serial.println(user);
    // });

    // server.on(UriRegex("^\\/users\\/([0-9]+)\\/devices\\/([0-9]+)$"), []() {
    // e.g. http://espsnap.local/users/53/devices/3444/0
    server.on(UriRegex("^/users/([0-9]+)/devices/([0-9]+)/([0-1])$"), []() {
      // Serial.println(server.pathArg(0));
      // Serial.println(server.pathArg(1));
      String user = server.pathArg(0);
      String device = server.pathArg(1);
      String value = server.pathArg(2);
      server.send(200, "text/plain", "User: '" + user + "' and Device: '" + device + "'and value: '" + value + "'");
    });

    server.on("/config", handleConfigForm);
    server.on("/postform/", handleConfig);
    server.on("/configplain", handleConfigPlain);
    server.on("/postplain/", handlePlain);

    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("HTTP server started");

    #ifdef SERIAL_VERBOSE
      Serial.println("SERIAL_VERBOSE mode");
    #endif SERIAL_VERBOSE

}


void loop(void) {
  server.handleClient();

  //////
  // only for ESP8266
  #ifdef ESP8266
    MDNS.update();

    // add this call to 120s interval MDNS.announce() to keep mDNS alive
    // otherwise, mDNS will timeout after ~120s
    if ((millis() - last_announce) > announce_interval) {
      // if (hMDNSService) {
        // Just trigger a new MDNS announcement, this will lead to a call to
        // 'MDNSDynamicServiceTxtCallback', which will update the time TXT item
        MDNS.announce();
        Serial.println("MDNS.announce()");
      // }
      last_announce = millis();
    }
  #endif ESP8266
  //////

  neopixel_process_update(); // update neopixel
  tune_process_update(); // update tune
  rotary_encoder_process_update(); // update rotary encoder
  delay(1);
}

