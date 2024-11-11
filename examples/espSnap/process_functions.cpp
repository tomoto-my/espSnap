# include "process_functions.h"

// #define SERIAL_VERBOSE

#ifdef ESP8266
  extern ESP8266WebServer server();
#endif ESP8266

#ifdef ESP32
  // extern NetworkServer server();
  extern WebServer server();
#endif ESP32



// Process Functions 
// Tone
// DHT22
// HC-SR04
// DS18B20
// NeoPixel
// I2C LCD 16x2
// OLED SSD1306
//
// Response format is
// line 1 : OK / NAK
// line 2 : device/function name
// line 3 : value/reading (comma separated)
// line 4 : unit / error message
// respond
//
// where there is no reading expected or ERROR, line 3 value will be
// 0 : good
// -ve : bad
// -1 : problem with hardware with different error message
// -2 : problem with hardware with different error message
// -3 : problem with hardware with different error message
// -4 : problem with hardware with different error message
// -5 : command problem (not likely if using the functions Blocks provided in Snap!)
// -6 : command problem (not likely if using the functions Blocks provided in Snap!)
// -7 : command problem (not likely if using the functions Blocks provided in Snap!)
// -8 : command problem (not likely if using the functions Blocks provided in Snap!)
// -9 : command problem (not likely if using the functions Blocks provided in Snap!)
// -10 : data supplied not valid
// -99 : undefined



// Tone processing handler
// parameter format - pin,frequency,duration
int tone_pin=-1;
int tone_process_cmd(String cmd_str) {
  // String cmd_str = *command_str;
  // log_d("tone_process_cmd:%s", cmd_str);

  String pin_str, freq_str, duration;
  int new_pin;
  int pos2 = 0;
  cmd_str = urldecode(cmd_str);

  pos2 = cmd_str.indexOf(',');
  if (pos2 > 0) {
    pin_str = cmd_str.substring(0, pos2);
    new_pin = pin_str.toInt();
    cmd_str = cmd_str.substring(pos2+1);

    pos2 = cmd_str.indexOf(',');
    if (pos2 > 0) {
      pos2 = cmd_str.indexOf(',');
      freq_str = cmd_str.substring(0, pos2);
      duration = cmd_str.substring(pos2+1);

      if ((new_pin != tone_pin) && (tone_pin != -1)) {
        // Serial.println("noTone");
        noTone(tone_pin); // this will release the previous tone pin if any
        delay(100);
        tone_pin = new_pin;
      }
      // log_d("tone - %s : %s : %s", pin, freq, duration);
      tone(new_pin, (uint)freq_str.toInt(), (long)duration.toInt());
      tone_pin = new_pin;
    }
    else if (cmd_str == "notone") {
      // notone
      // log_d("tone - notone");
      if ((new_pin == tone_pin) && (tone_pin != -1)) {
        // Serial.println("noTone");
        noTone(pin_str.toInt());
        tone_pin = -1;
      }
    }
  }

  return 0;
}



// Tune processing handler
// parameter format - pin,[frequency,duration,....]

String Notes_Name[] = {"A0", "AS0", "B0",
      "C1", "CS1", "D1", "DS1", "E1", "F1", "FS1", "G1", "GS1", "A1", "AS1", "B1", 
      "C2", "CS2", "D2", "DS2", "E2", "F2", "FS2", "G2", "GS2", "A2", "AS2", "B2", 
      "C3", "CS3", "D3", "DS3", "E3", "F3", "FS3", "G3", "GS3", "A3", "AS3", "B3", 
      "C4", "CS4", "D4", "DS4", "E4", "F4", "FS4", "G4", "GS4", "A4", "AS4", "B4", 
      "C5", "CS5", "D5", "DS5", "E5", "F5", "FS5", "G5", "GS5", "A5", "AS5", "B5", 
      "C6", "CS6", "D6", "DS6", "E6", "F6", "FS6", "G6", "GS6", "A6", "AS6", "B6", 
      "C7", "CS7", "D7", "DS7", "E7", "F7", "FS7", "G7", "GS7", "A7", "AS7", "B7", 
      "C8", "CS8", "D8", "DS8"
};

int Notes_Frequency[] = { NOTE_A0, NOTE_AS0, NOTE_B0, 
      NOTE_C1, NOTE_CS1, NOTE_D1, NOTE_DS1, NOTE_E1, NOTE_F1, NOTE_FS1, NOTE_G1, NOTE_GS1, NOTE_A1, NOTE_AS1, NOTE_B1, 
      NOTE_C2, NOTE_CS2, NOTE_D2, NOTE_DS2, NOTE_E2, NOTE_F2, NOTE_FS2, NOTE_G2, NOTE_GS2, NOTE_A2, NOTE_AS2, NOTE_B2, 
      NOTE_C3, NOTE_CS3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_AS3, NOTE_B3, 
      NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4, 
      NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5, 
      NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6, 
      NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7,
      NOTE_C8, NOTE_CS8, NOTE_D8, NOTE_DS8
};

#define TUNE_MAX_NOTES 100

boolean tune_init = false;
boolean flag_tune_run = false;
boolean flag_tune_interval = false;
int tune_loop_delay = 0;
long tune_last_update = 0;
int tune_update_check_count = 0;
int tune_current_pos = 0;
int tune_notes[TUNE_MAX_NOTES];
int tune_durations[TUNE_MAX_NOTES];
int tune_notes_count = 0;
int tune_pin = -1;

int note_pitch_to_freq(String notestr) {
  // Serial.println(notestr);
  int note_pitch_len = sizeof(Notes_Frequency)/sizeof(int);
  // Serial.println(note_pitch_len);

  // for (int i=0; i<note_pitch_len; i++){
  //   log_d("%d : %s : %d", i, Notes_Name[i], Notes_Frequency[i]);
  // }
  for (int i=0; i<note_pitch_len; i++){
    if (notestr == Notes_Name[i]) {
      // log_d("MATCHED : %d : %s : %d", i, Notes_Name[i], Notes_Frequency[i]);
      return Notes_Frequency[i];
    }
  }
  return 0;
}

int tune_process_update() {

  if (!flag_tune_run) return -1;
  if ((millis() - tune_last_update) > tune_loop_delay) {
    if(tune_current_pos >= tune_notes_count-1) {
      // tune ended
      noTone(tune_pin);
      flag_tune_run = false;
      tune_current_pos = 0;
      return 0;
    }

    if(flag_tune_interval) {
      tune_last_update = millis();
      tune_loop_delay = 10; // short 10ms gap
      flag_tune_interval = false;
      return 0;
    }
    tune_loop_delay = tune_durations[++tune_current_pos]; // ms
    // noTone(tune_pin);
    tone(tune_pin, tune_notes[tune_current_pos], tune_loop_delay);

    tune_last_update = millis();
    flag_tune_interval = true;
    tune_update_check_count = 0;
    // Serial.println("");
  }
  else {
    tune_update_check_count++;
    // if ((tune_update_check_count % 50) == 0) 
    //   Serial.print(".");
  }
  return tune_current_pos;
}

int tune_process_cmd(String cmd_str) {
  // String cmd_str = *command_str;
  // log_d("tune_process_cmd:%s", cmd_str);

  String pin, freq, duration;
  int pos2 = 0;

  char *pch;
  char *cptr[TUNE_MAX_NOTES+1];
  int param_count = 0;
  pch = strtok((char*)cmd_str.c_str(), ",");
  while (pch != NULL) {
    if (param_count>TUNE_MAX_NOTES) {
      pch = NULL;
    }
    else {
      cptr[param_count++] = pch;
      // log_d("parameter:%p:%s",pch,pch);
      pch = strtok(NULL, ",");
    }
  }
  // log_d("param count=%d",param_count);

  String pinstr = String(cptr[0]);
  int idx = 1;
  int tune_idx = 0;
  while ((idx<param_count) && (idx<TUNE_MAX_NOTES)) {
    String notestr = String(cptr[idx]);
    String durationstr = String(cptr[idx+1]);
    tune_notes[tune_idx] = note_pitch_to_freq(notestr);
    tune_durations[tune_idx] = durationstr.toInt();
    // log_d("notes : %d : %d", tune_notes[tune_idx], tune_durations[tune_idx]);
    idx += 2;
    tune_idx++;
  } 
  tune_notes_count = tune_idx;

  if(tune_notes_count > 0) {
    // start play tune
    tune_pin = pinstr.toInt();
    tune_loop_delay = tune_durations[0]; // ms
    tone(tune_pin, tune_notes[0], tune_loop_delay);
    flag_tune_run = true;
    tune_current_pos = 0;
    tune_last_update = millis();
    flag_tune_interval = true;
  }



  return 0;
}




// DHT22 processing handler
void DHT22_process_cmd(int DataPin, char *resp_buf) {
  String resp="";
  DHT22 dht22(DataPin);
  float t = dht22.getTemperature();
  float h = dht22.getHumidity();
  String resultstr;
  if (dht22.getLastError() != dht22.OK) { // invalid
    // Serial.print("Invalid Response - ");
    // Serial.print("last error :");
    int lasterror = dht22.getLastError();
    // Serial.println(lasterror);
    resultstr = "Invalid Response - last error : " + String(lasterror) + "\n";
    resp = "NAK\nDHT22\n-1\n" + resultstr;
  }
  else {
    // Serial.print("h=");Serial.print(h,1);Serial.print("\t");
    // Serial.print("t=");Serial.println(t,1);
    char temp_buf[15];
    char hum_buf[15];
    floatToString(t, temp_buf, sizeof(temp_buf), 4);
    floatToString(h, hum_buf, sizeof(hum_buf), 4);
    resultstr = String(temp_buf) + "," + String(hum_buf) + "\n";
    resp = "OK\nDHT22\n" + resultstr + "C,%\n";
  }

  // Response format is
  // line 1 : OK / NAK
  // line 2 : device/function name
  // line 3 : value/reading (comma separated)
  // line 4 : unit / error message
  strcpy(resp_buf, resp.c_str());
}





// HC-SR04 processing handler
float HCSR04_process_cmd(int trigPin, int echoPin) {
  // define sound velocity in cm/uS
  // #define SOUND_VELOCITY 0.034
  // #define CM_TO_INCH 0.393701
  // speed of sound wave in air = 343 m/s = 343*1e3/1e6 == 0.343 mm/us
  #define SOUND_VELOCITY 0.343 // mm/s
  #define MM_TO_INCH 0.0393701

  long duration;
  float distancemm;
  float distanceInch;
  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distancemm = duration * SOUND_VELOCITY/2;
  
  // Convert to inches
  // distanceInch = distancemm * MM_TO_INCH;
  
  // Prints the distance on the Serial Monitor
  #ifdef SERIAL_VERBOSE
    Serial.print("Duration (us): ");
    Serial.println(duration);
    Serial.println(distancemm);
    Serial.print("Distance (mm): ");
    Serial.println(distancemm);
    // Serial.print("Distance (inch): ");
    // Serial.println(distanceInch);
  #endif SERIAL_VERBOSE

  return distancemm;

}





// DS18B20 processing handler
// Response format is
// line 1 : OK / NAK
// line 2 : device/function name
// line 3 : value/reading (comma separated)
// line 4 : unit / error message
#include <OneWire.h>
#include <DallasTemperature.h>
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
// OneWire oneWire(ONE_WIRE_BUS);
OneWire oneWire();

// Pass our oneWire reference to Dallas Temperature. 
// DallasTemperature ds18b20_sensors(&oneWire);
DallasTemperature ds18b20_sensors();

// arrays to hold device address
DeviceAddress insideThermometer;

void ds18b20_process_cmd(int DataPin, char *resp_buf) {
    // OneWire oneWire(DataPin);
  #ifdef SERIAL_VERBOSE
    Serial.println("ds18b20_process_cmd");
    Serial.println((unsigned long) resp_buf);
  #endif
  OneWire oneWire(DataPin);
  DallasTemperature ds18b20_sensors(&oneWire);
  // arrays to hold device address
  // DeviceAddress insideThermometer;

  ds18b20_sensors.begin();

  #ifdef SERIAL_VERBOSE
    // Serial.print("Found ");
    Serial.print(ds18b20_sensors.getDeviceCount(), DEC);
    // Serial.println(" devices.");
  #endif SERIAL_VERBOSE

  // report parasite power requirements
  // Serial.print("Parasite power is: "); 
  // if (ds18b20_sensors.isParasitePowerMode()) Serial.println("ON");
  // else Serial.println("OFF");

  String resp="";
  if (!ds18b20_sensors.getAddress(insideThermometer, 0)) {
    // Serial.println("Unable to find address for Device 0"); 
    String resultstr = "Error: Unable to find address for Device 0\n"; 
    resp = "NAK\nDS18B20\n-1\n" + resultstr;
  } 
  else { 
    // // show the addresses we found on the bus
    // Serial.print("Device 0 Address: ");
    // printAddress(insideThermometer);
    // Serial.println();

    // set the resolution to 12 bit (Each Dallas/Maxim device is capable of several different resolutions)
    ds18b20_sensors.setResolution(insideThermometer, 12);
  
    // Serial.print("Device 0 Resolution: ");
    // Serial.print(ds18b20_sensors.getResolution(insideThermometer), DEC); 
    // Serial.println();

    // Serial.print("Requesting temperatures...");
    ds18b20_sensors.requestTemperatures(); // Send the command to get temperatures
    // Serial.println("DONE");
      
    float tempC = ds18b20_sensors.getTempC(insideThermometer);
    if(tempC == DEVICE_DISCONNECTED_C) 
    {
      // Serial.println("Error: Could not read temperature data");
      String resultstr = "Error: Could not read temperature data\n";
      resp = "NAK\nDS18B20\n-2\n" + resultstr;
    }
    else {
      // Serial.print("Temp C: ");
      // Serial.print(tempC);
      // Serial.print(" Temp F: ");
      // Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit

      char tempc_buf[15];
      // char tempf_buf[15];
      floatToString(tempC, tempc_buf, sizeof(tempc_buf), 4);
      // floatToString(DallasTemperature::toFahrenheit(tempC), tempf_buf, sizeof(tempf_buf), 4);

      // String resultstr = "Temp C: " + String(tempc_buf) + "\nTemp F: " + String(tempf_buf) + "\n";
      String resultstr = String(tempc_buf) + "\nC\n";
      resp = "OK\nDS18B20\n" + resultstr;
    }
  }

  // Response format is
  // line 1 : OK / NAK
  // line 2 : device/function name
  // line 3 : value/reading (comma separated)
  // line 4 : unit / error message
  strcpy(resp_buf, resp.c_str());
}







// Adafruit NeoPixel library
#include <Adafruit_NeoPixel.h>

// #define LED_PIN    15

// How many NeoPixels are attached to the Arduino?
// #define LED_COUNT 15

// #define LED_BRIGHTNESS 10

#define NEOPIXEL_MAX_PIXELS 300
#define NEOPIXEL_MAX_BUFFER 4000

int neopixel_pin;
int neopixel_count;
int neopixel_run_count;
boolean neopixel_init = false;
boolean flag_neopixel_run = false;
int neopixel_current_pos = -1;
int neopixel_loop_delay = 0;
unsigned long neopixel_last_update;
char neopixel_buffer[NEOPIXEL_MAX_BUFFER];
int neopixel_buffer_length = 0;

// init Adafruit NeoPixel variable 
// Declare our NeoPixel strip object:
// Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Adafruit_NeoPixel strip(neopixel_count, neopixel_pin, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)          
// in this case, it is assumed to be WS2812 with GRB value
// if RGB value instead, then user need to change the RGB in Snap! for now
// Adafruit_NeoPixel strip(neopixel_count, neopixel_pin, NEO_GRB + NEO_KHZ800);
// Adafruit_NeoPixel strip(15, 15, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip; // call at compile time without initialize pin & count

// void colorWipe(uint32_t color, int wait) {
//   for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
//     strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
//     strip.show();                          //  Update strip to match
//     delay(wait);                           //  Pause for a moment
//   }
// }

unsigned char neopixel_h2int(char c)
{
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}

long neopixel_update_check_count = 0;
int neopixel_process_update() {

  if (!flag_neopixel_run) return -1;
  if ((millis() - neopixel_last_update) > neopixel_loop_delay) {
    // Serial.println("neopixel update");

    int start_idx = neopixel_current_pos;
    // Serial.print(neopixel_current_pos);
    // Serial.print(",");
    // Serial.print(neopixel_buffer_length);
    // Serial.println("");

    int r,g,b;
    for (int p = 0; p < neopixel_run_count; p++) {
      int idx = start_idx++ * 6;
      // Serial.println(idx);
      if ((start_idx*6) >= neopixel_buffer_length) {
        start_idx = 0;
          idx = 0;
      }
      r = (h2int(neopixel_buffer[idx]) << 4) | h2int(neopixel_buffer[idx+1]);
      idx += 2;
      g = (h2int(neopixel_buffer[idx]) << 4) | h2int(neopixel_buffer[idx+1]);
      idx += 2;
      b = (h2int(neopixel_buffer[idx]) << 4) | h2int(neopixel_buffer[idx+1]);
      strip.setPixelColor(p, strip.Color(r,g,b));
    }
    strip.show();                          //  Update strip to match

    if (++neopixel_current_pos >= (neopixel_buffer_length / 6)) {
      neopixel_current_pos = 0;
    }
    
    neopixel_last_update = millis();
    neopixel_update_check_count = 0;
    // Serial.println("");
  }
  else {
    neopixel_update_check_count++;
    // if ((neopixel_update_check_count % 50) == 0) 
      // Serial.print(".");
  }
  return neopixel_current_pos;
}


void neopixel_process_init(int pin, int count) {
  // Declare our NeoPixel strip object:
  // Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

  // strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  // strip.show();            // Turn OFF all pixels ASAP
  // strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  // neopixel_init = true;
}

// NeoPixel processing handler
// NeoPixel parameter format - comma separated values
//   start, pin, count
//   fillall, option
//   set, count, colors-RRGGBB
//   run, count, delay, colors-RRGGBB
//   stoprun
//   brightness, value
// Color Wheel parameter format - comma separated values
//   steps = 12/18/24/36
int neopixel_process_cmd(String cmd_str) {
  // log_d("neopixel_process:%d:%p:%s", cmd_str.length(), cmd_str, cmd_str);

  char *pch;
  char *cptr[4];
  int param_count = 0;
  pch = strtok((char*)cmd_str.c_str(), ",");
  while (pch != NULL) {
    cptr[param_count++] = pch;
    // log_d("parameter:%p:%s",pch,pch);
    pch = strtok(NULL, ",");
  }
  // log_d("param count=%d",param_count);
  // for (int i=0; i<param_count; i++)
  //   log_d("param_cptr%d=%s", i, cptr[i]);


  if (param_count == 1) { // command with no parameter
    if (strcmp(cptr[0], "stoprun") == 0) {
      flag_neopixel_run = false;
      neopixel_current_pos = -1;
      return 0;
    }
  }
  else if (param_count == 2) { // brightness & fillall
    if (strcmp(cptr[0], "brightness") == 0) {
      int brightness = String(cptr[1]).toInt();
      if (brightness <= 0) brightness = 1;
      if (brightness > 255) brightness = 255;
      strip.setBrightness(brightness); // Set BRIGHTNESS (max = 255)
      strip.show();
      return 0;
    }
    else if (strcmp(cptr[0], "fillall") == 0) {
      //////
      // only for ESP8266
      #ifdef ESP8266
        uint32 out_color;
      #endif ESP8266
      // for ESP 32
      #ifdef ESP32
        uint32_t out_color;
      #endif ESP32
      //////
      char *pch = cptr[1];
      uint8_t r = (h2int(*pch++) << 4) | h2int(*pch++);
      uint8_t g = (h2int(*pch++) << 4) | h2int(*pch++);
      uint8_t b = (h2int(*pch++) << 4) | h2int(*pch++);
      out_color = strip.Color(r, g, b);

      for(int i=0; i<neopixel_count; i++) { // For each pixel...
        // strip.setPixelColor(i, strip.Color(0, 150, 0));
        strip.setPixelColor(i, out_color);
      }
      strip.show();   // Send the updated pixel colors to the hardware.
      return 0;
    }
  }
  else if (param_count == 3) { // set & start
    if (strcmp(cptr[0], "set") == 0) { // set color at pixel and leave 
      // Serial.println("neopixel.set");
      if (!neopixel_init) return -1; // MUST initialize first
      int num_pixel = String(cptr[1]).toInt(); // num of pixel is 1st parameter
      

      if (num_pixel > 300) return -5; // just an arbitrary limit
      
      if (num_pixel > neopixel_count) return -6; // cannnot set more than initialized

      int valuelen = strlen(cptr[2]);
      if (valuelen < num_pixel * 2 * 3) return -7; // number of GRB value is not enough

      char r, g, b;
      char *cptr2 = cptr[2];
      for (int p = 0; p < num_pixel; p++) {
        int idx = p * 6;
        r = (h2int(*(cptr2+idx)) <<4 ) | h2int(*(cptr2+idx+1));
        idx += 2;
        g = (h2int(*(cptr2+idx)) << 4) | h2int(*(cptr2+idx+1));
        idx += 2;
        b = (h2int(*(cptr2+idx)) << 4) + h2int(*(cptr2+idx+1));
        // log_d("r=0x%x, g=0x%x, b=0x%x",r,g,b);
        strip.setPixelColor(p, strip.Color(r,g,b));
      }
      // strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
      strip.show();                          //  Update strip to match
      return 0;
    }
    else if (strcmp(cptr[0], "start") == 0) {
      // Serial.println("neopixel.start");
      int pin_num = String(cptr[1]).toInt();
      int num_pixel = String(cptr[2]).toInt();

      if (num_pixel > NEOPIXEL_MAX_PIXELS) return -5; // just an arbitrary limit
      
      neopixel_pin = pin_num;
      neopixel_count = num_pixel;

      // NEED to call this to set length 
      // because not initialized during compile time
      strip.updateLength(neopixel_count); 
      strip.setPin(neopixel_pin);
      strip.updateType(NEO_GRB + NEO_KHZ800); // can update type here also
      // strip.updateType(NEO_RGB + NEO_KHZ800); // can update type here also

      strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)

      strip.clear(); // Set all pixel colors to 'off'
      strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
      strip.show();            // Turn OFF all pixels ASAP
      neopixel_init = true;
      return 0;
    }
  }
  else if (param_count == 4) {
    if (strcmp(cptr[0], "run") == 0) { // will update color from value received in a continous loop
      // Serial.println("neopixel.run");
      if (!neopixel_init) return -1; // MUST initialize first
      int num_pixel = String(cptr[1]).toInt(); // num of pixel is 1st parameter

      if (num_pixel > NEOPIXEL_MAX_PIXELS) return -5; // just an arbitrary limit
      
      if (num_pixel > neopixel_count) return -6; // cannnot set more than initialized
      neopixel_run_count = num_pixel;

      String delay_str = String(cptr[2]);
      String value_str = String(cptr[3]);
      int valuelen = value_str.length();
      if (valuelen < num_pixel * 2 * 3) return -7; // number of GRB value is not enough

      // log_d("%d:%s:%s", num_pixel, delay_str, cptr[3]);

      if (valuelen > NEOPIXEL_MAX_PIXELS * 2 * 3) {
        // trim to max
        value_str = value_str.substring(0,NEOPIXEL_MAX_PIXELS*2*3);
        valuelen = value_str.length();
      }
      if ((valuelen % 6) != 0)  {
        // truncate to match multiple of 6 characters
        int trimcount = (valuelen / 6) * 6;
        value_str = value_str.substring(0,trimcount);
      }

      uint r, g, b;
      char *cptr2 = cptr[3];
      for (int p = 0; p < neopixel_run_count; p++) {
        int idx = p * 6;
        r = (h2int(*(cptr2+idx)) <<4 ) | h2int(*(cptr2+idx+1));
        idx += 2;
        g = (h2int(*(cptr2+idx)) << 4) | h2int(*(cptr2+idx+1));
        idx += 2;
        b = (h2int(*(cptr2+idx)) << 4) + h2int(*(cptr2+idx+1));
        // log_d("r=0x%x, g=0x%x, b=0x%x",r,g,b);
        strip.setPixelColor(p, strip.Color(r,g,b));
      }
      strip.show();      
      neopixel_loop_delay = delay_str.toFloat() * 1000; // s to ms
      neopixel_current_pos = 0;
      neopixel_last_update = millis();
      neopixel_buffer_length = value_str.length();
      strcpy(neopixel_buffer, value_str.c_str());
      flag_neopixel_run = true;
      return 0;
    }
    else if (strcmp(cptr[0], "fillrgb") == 0) {
      //////
      // only for ESP8266
      #ifdef ESP8266
        uint32 out_color;
      #endif ESP8266
      // for ESP 32
      #ifdef ESP32
        uint32_t out_color;
      #endif ESP32
      //////
      uint8_t r = strtol(cptr[1], 0, 16);
      uint8_t g = strtol(cptr[2], 0, 16);
      uint8_t b = strtol(cptr[3], 0, 16);
      // Serial.println(r);
      // Serial.println(g);
      // Serial.println(b);
      // log_d("r=%d, g=%d, b=%d", r, g, b);
      out_color = strip.Color(r, g, b);

      for(int i=0; i<neopixel_count; i++) { // For each pixel...
        // strip.setPixelColor(i, strip.Color(0, 150, 0));
        strip.setPixelColor(i, out_color);
      }
      strip.show();   // Send the updated pixel colors to the hardware.
      return 0;

    }
  }
  return -99; // undefined
}




// ESP32 Wemos D1 R32 default pin is
// GPIO22 : SCL
// GPIO21 : SDA
// ESP8266 I2C default pin is
// D1 : GPIO5 : SCL
// D2 : GPIO4 : SDA
//
// set the LCD number of columns and rows
// int lcdColumns = 16;
// int lcdRows = 2;
//
// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
//
// I2C_LCD
// =======
// due to HD44780U character rom pattern
// ~ : show as right arrow character
// \ : show as Japanese dollar sign
//
// characters need to be send in as urlencoded
// but will show up correctly
// % : not allowed by http : send as "%25%25" from Snap!
// # : truncated line : send as "%23" from Snap!
// ? : truncated line : send as "%3F" from Snap!
// , : send as "%2C" from Snap!
// // \ : %5C but HD44780 rom character pattern is "Yen" sign 
// \ changed to %25 to specify custom characters 1-8
// / : send as "%2F" from Snap!
//
bool I2CLCD_display_init = false;

byte I2CLCD_dotOff[] = { 0b00000, 0b01110, 0b10001, 0b10001,
                        0b10001, 0b01110, 0b00000, 0b00000 };
byte I2CLCD_dotOn[] = { 0b00000, 0b01110, 0b11111, 0b11111,
                        0b11111, 0b01110, 0b00000, 0b00000 };
byte I2CLCD_tick[] = { 0b00001, 0b00001, 0b00010, 0b00010,
                        0b10100, 0b10100, 0b01000, 0b01000 };
byte I2CLCD_custom1[] = { 0b00001, 0b00001, 0b00010, 0b00010,
                        0b10100, 0b10100, 0b01000, 0b01000 };

// LiquidCrystal_I2C lcd(I2C_LCD_ADDRESS, I2C_LCD_COLUMNS, I2C_LCD_ROWS);  
LiquidCrystal_PCF8574 lcd(I2C_LCD_ADDRESS);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// process I2C LCD commands
int i2c_lcd_process_cmd(String cmd_str) {
  // log_d("i2c_lcd_process:%p:%s",cmd_str, cmd_str);
  // Serial.println(cmd_str);

  char *pch;
  char *cptr[10];
  int param_count = 0;
  pch = strtok((char*)cmd_str.c_str(), ",");
  while (pch != NULL) {
    cptr[param_count++] = pch;
    // log_d("parameter:%p:%s",pch,pch);
    pch = strtok(NULL, ",");
  }
  // log_d("param count=%d",param_count);
  // for (int i=0; i<param_count; i++)
  //   log_d("param_cptr%d=%s", i, cptr[i]);

  int result = 0; // assume ok
  
  if(!I2CLCD_display_init) {
    // if (strcmp(cptr[0], "start") == 0) {
      // auto initialize I2C_LCD
      // log_d("init i2c lcd - auto");
      lcd.begin(I2C_LCD_COLUMNS, I2C_LCD_ROWS);  // initialize the lcd

      lcd.setBacklight(255);
      lcd.home();
      lcd.clear();
      I2CLCD_display_init = true;
    // }
    // else {
    //   return -5;
    // }
  }


  if(param_count == 1) { // command with no parameter
    if (strcmp(cptr[0], "start") == 0) {
      // log_d("init i2c lcd");
      // Serial.println("init i2c lcd");
      // lcd.init();
      // lcd.clear(); // clear screen
      // lcd.home(); // set position of 0,0
      // lcd.backlight();
      lcd.begin(I2C_LCD_COLUMNS, I2C_LCD_ROWS);  // initialize the lcd

      lcd.setBacklight(255);
      lcd.home();
      lcd.clear();
      // lcd.print("LCD Custom");
      // lcd.createChar(1, I2CLCD_dotOff); // define custom character #1
      // lcd.createChar(2, I2CLCD_dotOn);  // define custom chracter #2
      // lcd.createChar(3, I2CLCD_tick);  // define custom chracter #2
      // lcd.setCursor(0, 1);
      // lcd.print("1:\01"); 
      // lcd.setCursor(5, 1);
      // lcd.print("2:\02");
      // lcd.setCursor(10, 1);
      // lcd.print("3:\03");
    }
    else if (strcmp(cptr[0], "clear") == 0) {
      // Serial.println("clear");
      lcd.clear();
      // delay(100); // added a delay otherwise user need to add this manually
    }
    else if (strcmp(cptr[0], "backlight") == 0) {
      // Serial.println("backlight");
      // lcd.backlight();
      lcd.setBacklight(255);
    }
    else if (strcmp(cptr[0], "nobacklight") == 0) {
      // Serial.println("nobacklight");
      // lcd.noBacklight();
      lcd.setBacklight(0);
    }
    else if (strcmp(cptr[0], "cursor") == 0) {
      // Serial.println("cursor");
      lcd.cursor();
    }
    else if (strcmp(cptr[0], "nocursor") == 0) {
      // Serial.println("nocursor");
      lcd.noCursor();
    }
    else if (strcmp(cptr[0], "blink") == 0) {
      // Serial.println("blink");
      lcd.blink();
    }
    else if (strcmp(cptr[0], "noblink") == 0) {
      // Serial.println("noblink");
      lcd.noBlink();
    }
    else if (strcmp(cptr[0], "displayon") == 0) {
      // Serial.println("noblink");
      lcd.display();
    }
    else if (strcmp(cptr[0], "displayoff") == 0) {
      // Serial.println("noblink");
      lcd.noDisplay();
    }
  }
  else if (param_count == 4) { // command with 3 parameters
      if (strcmp(cptr[0], "print") == 0) {
        // "\" printed as "Yen" sign due to HD44780U character rom pattern
        // "\" used to specify custom characters '\01' to '\07'
        String columns = String(cptr[1]);
        String rows = String(cptr[2]);
        String message = urldecode(String(cptr[3]));
        // Serial.print("message=[");
        // Serial.print(message);
        // Serial.println("]");
        // // log_d("i2c_lcd_print:%s:%s:[%s]", columns, rows, message);

        lcd.setCursor(columns.toInt(), rows.toInt());
        int pos2 = message.length() - 1; 
        if (pos2<0) pos2 = 0;
        message = message.substring(1, pos2);

        // Serial.println(message);
        lcd.print(message);
      }
      else { // invalid 3 parameter command
        result = -7;
      }
  }
  else if (param_count == 10) { // command with 9 parameters
      if (strcmp(cptr[0], "customchar") == 0) {
        // Serial.println("i2c_lcd_customchar");
        byte custom_char[8];
        // for(int i=0; i<8; i++) {
        //   // Serial.println(I2CLCD_custom1[i] + '0');
        //   Serial.println(I2CLCD_custom1[i], HEX);
        // }
        for(int i=0; i<8; i++) {
          // Serial.print(cptr[i+2]);
          // Serial.print(":");
          char *bptr = cptr[i+2];
          uint row_value = 0;
          for(int j=0; j<5; j++) {
            row_value = (row_value << 1);
            // Serial.print(*(bptr+j));
            // Serial.print(":");
            if (*(bptr+j) != '0') {
              row_value += 1;
            }
          }
          // row_value=0x11;
          I2CLCD_custom1[i] = row_value;
          // Serial.println(row_value, HEX);
          // Serial.println(I2CLCD_custom1[i], HEX);
        }
        int idx = String(cptr[1]).toInt();
        // Serial.println(idx);
        lcd.createChar(idx, I2CLCD_custom1);
        // lcd.setCursor(0, 1);
        // lcd.print(String(cptr[1]));
        // lcd.print(String(cptr[1]) + ":" + ((char) (idx)));        // lcd.print(String(cptr[1]));
      }
      else { // invalid 9 parameter command
        result = -8;
      }
  } 
  else{ // invalid 1 & 2 parameter command
    result = -6;
  }
  return result;
}





// ESP32 Wemos D1 R32 default pin is
// GPIO22 : SCL
// GPIO21 : SDA
// ESP8266 I2C default pin is
// D1 : GPIO5 : SCL
// D2 : GPIO4 : SDA
//
// start
// clear - display.clearDisplay()
// invert    - void invertDisplay(bool i);
// drawpixel - void drawPixel(int16_t x, int16_t y, uint16_t color);
// drawline  - void Adafruit_GFX::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
// drawcircle - void Adafruit_GFX::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
// drawrect - void Adafruit_GFX::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
// drawroundrect - void Adafruit_GFX::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
// drawtriangle - void Adafruit_GFX::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
// fillcircle - void Adafruit_GFX::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
// fillrect - void Adafruit_GFX::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
// fillroundrect - void Adafruit_GFX::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
// filltriangle -void Adafruit_GFX::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
// settextsize - void setTextSize(uint8_t s);
// setcursor - void setCursor(int16_t x, int16_t y);
// settextcolor - void setTextColor(uint16_t c, uint16_t bg); // SSD1306_BLACK / SSD1306_WHITE /SSD1306_INVERSE
// println - println(String); // at cursor position
// void Adafruit_GFX::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
// loadbitmap - loadbitmap for drawbitmap
// drawbitmap - void Adafruit_GFX::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);
// 
//
/// fit into the SSD1306_ naming scheme
// color
// #define SSD1306_BLACK 0   ///< Draw 'off' pixels
// #define SSD1306_WHITE 1   ///< Draw 'on' pixels
// #define SSD1306_INVERSE 2 ///< Invert pixels
//
// exception in "/println/" command
// % : not allowed by http : send as "%25%25" from Snap!
// # : truncated line : send as "%23" from Snap!
// ? : truncated line : send as "%3F" from Snap!
// , : send as "%2C" from Snap!
// / : send as "%2F" from Snap!
// \ : output as / instead : need to send as "%7F" from Snap!
// all 6 characters needs to be encoded to hex value in Snap!
// all characters display correctly after encoded to hex value in Snap!



//
// void print_ptr(String *value) {
//   /* initialisation */
//   char string[]="0123456789ABCDEF";       // array of characters corresponding to numbers from 0 to 15
//   uint16_t ptr;

//   /* these lines go inside the loop */
//   ptr = (uint16_t) &value;                // store 16-bit address of 'value'
//   putchar( string[ (ptr >> 12) & 0xF ] ); // Write out highest 4-bits of memory address
//   putchar( string[ (ptr >>  8) & 0xF ] );
//   putchar( string[ (ptr >>  4) & 0xF ] );
//   putchar( string[ (ptr >>  0) & 0xF ] ); // Write out lowest 4-bits of memory address
// }
Adafruit_SSD1306 display(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire, OLED_RESET);
bool OLED_display_init = false;
u_char OLED_custom_bmp[32];

int i2c_oled_process_cmd(String cmd_str) {
  // log_d("i2c_oled_process:%p:%s",cmd_str, cmd_str);
    // Serial.println(cmd_str);
  int result = 0; // assume ok
  String command;
  int x0,y0,x1,y1,x2,y2,r0;
  int w,h,color;

  if (cmd_str == "start") {
    // Serial.println("oled start");
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_SCREEN_ADDRESS)) {
      // Serial.println(F("SSD1306 allocation failed"));
      return -1;
    }
    OLED_display_init = true;

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display.display();
    // delay(200);
    return 0;
  }
  
  if (!OLED_display_init) { // auto initialize of oled - OR can change to auto ignore
    // Serial.println("display not initialized");
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_SCREEN_ADDRESS)) {
      // Serial.println(F("SSD1306 allocation failed"));
      return -2;
    }
    // Serial.println("display initialized");
    OLED_display_init = true;
  }

  
  // extract parameters
  char *pch;
  // char *cptr[8];
  char *cptr[38];
  int param_count = 0;
  pch = strtok((char*)cmd_str.c_str(), ",");
  while (pch != NULL) {
    cptr[param_count++] = pch;
    // log_d("parameter:%p:%s",pch,pch);
    pch = strtok(NULL, ",");
  }
  // log_d("param count=%d",param_count);
  // for (int i=0; i<param_count; i++)
  //   log_d("param_cptr%d=%s", i, cptr[i]);

  command = String(cptr[0]);
  if (cmd_str == "clear") {
    // Serial.println("oled clear");
    // Clear the buffer
    display.clearDisplay();
    // display.setTextSize(1);
    display.setCursor(0, 0);
    display.setTextColor(1, 0);
    // update the screen with buffer content
    display.display();
    return 0;
  }
  else if (command == "println") { // command "println"
    // Serial.println("oled println");
    String message = urldecode(String(cptr[1]));
    // Serial.print("$$");
    // Serial.print(message);
    // Serial.println("$$$");
    display.println(message);
    display.display();
  }
  else if(command == "settextsize") {
    int size = String(cptr[1]).toInt();
    display.setTextSize(size);
  }
  else if(command == "setcursor") {
    x0 = String(cptr[1]).toInt();
    y0 = String(cptr[2]).toInt();
    display.setCursor(x0, y0);
  }
  else if(command == "settextcolor") {
    int textcolor = String(cptr[1]).toInt();
    if (param_count == 2) {
      display.setTextColor(textcolor);
    }
    else {
      int backcolor = String(cptr[2]).toInt();
      display.setTextColor(textcolor, backcolor);
    }
  }
  else if (command == "drawpixel") {
    x0 = String(cptr[1]).toInt();
    y0 = String(cptr[2]).toInt();
    color = (uint16_t) String(cptr[3]).toInt();
    display.drawPixel(x0, y0, color);
  }
  else if (command == "drawline") {
    // Serial.println("drawline");
    x0 = String(cptr[1]).toInt();
    y0 = String(cptr[2]).toInt();
    x1 = String(cptr[3]).toInt();
    y1 = String(cptr[4]).toInt();
    color = (uint16_t) String(cptr[5]).toInt();
    // color = SSD1306_WHITE;
    display.drawLine(x0, y0, x1, y1, color);
  }
  else if(command == "drawcircle") {
    x0 = String(cptr[1]).toInt();
    y0 = String(cptr[2]).toInt();
    r0 = String(cptr[3]).toInt();
    color = (uint16_t) String(cptr[4]).toInt();
    display.drawCircle(x0, y0, r0, color);
  }
  else if(command == "drawrect") {
    x0 = String(cptr[1]).toInt();
    y0 = String(cptr[2]).toInt();
    w  = String(cptr[3]).toInt();
    h  = String(cptr[4]).toInt();
    color = (uint16_t) String(cptr[5]).toInt();
    display.drawRect(x0, y0, w, h, color);
  }
  else if(command == "drawroundrect") {
    x0 = String(cptr[1]).toInt();
    y0 = String(cptr[2]).toInt();
    w  = String(cptr[3]).toInt();
    h  = String(cptr[4]).toInt();
    r0 = String(cptr[5]).toInt();
    color = (uint16_t) String(cptr[6]).toInt();
    display.drawRoundRect(x0, y0, w, h, r0, color);
  }
  else if(command == "drawtriangle") {
    x0 = String(cptr[1]).toInt();
    y0 = String(cptr[2]).toInt();
    x1 = String(cptr[3]).toInt();
    y1 = String(cptr[4]).toInt();
    x2 = String(cptr[5]).toInt();
    y2 = String(cptr[6]).toInt();
    color = (uint16_t) String(cptr[7]).toInt();
    display.drawTriangle(x0, y0, x1, y1, x2, y2, color);
  }
  else if(command == "fillcircle") {
    x0 = String(cptr[1]).toInt();
    y0 = String(cptr[2]).toInt();
    r0 = String(cptr[3]).toInt();
    color = (uint16_t) String(cptr[4]).toInt();
    display.fillCircle(x0, y0, r0, color);
  }
  else if(command == "fillrect") {
    x0 = String(cptr[1]).toInt();
    y0 = String(cptr[2]).toInt();
    w  = String(cptr[3]).toInt();
    h  = String(cptr[4]).toInt();
    color = (uint16_t) String(cptr[5]).toInt();
    display.fillRect(x0, y0, w, h, color);
  }
  else if(command == "fillroundrect") {
    x0 = String(cptr[1]).toInt();
    y0 = String(cptr[2]).toInt();
    w  = String(cptr[3]).toInt();
    h  = String(cptr[4]).toInt();
    r0 = String(cptr[5]).toInt();
    color = (uint16_t) String(cptr[6]).toInt();
    display.fillRoundRect(x0, y0, w, h, r0, color);
  }
  else if(command == "filltriangle") {
    x0 = String(cptr[1]).toInt();
    y0 = String(cptr[2]).toInt();
    x1 = String(cptr[3]).toInt();
    y1 = String(cptr[4]).toInt();
    x2 = String(cptr[5]).toInt();
    y2 = String(cptr[6]).toInt();
    color = (uint16_t) String(cptr[7]).toInt();
    display.fillTriangle(x0, y0, x1, y1, x2, y2, color);
  }
  else if(command == "setbmp") {
    // Serial.println(cmd_str);
    // Serial.println(param_count);
    w = String(cptr[1]).toInt();
    h = String(cptr[2]).toInt();

    if (w > 16) return -6; // max width 16
    if (h > 16) return -7; // max height 16

    int bmpidx = 0;
    int row_byte_count;
    if (w>8)
      row_byte_count = 2;
    else 
      row_byte_count = 1;

    for(int i=0; i<h; i++) {
      uint row_value = 0;
      char *bptr = cptr[3 + i*row_byte_count];
      int joffset = 0;
      for(int j=0; j<w; j++) {
        if(j==8) {
          OLED_custom_bmp[bmpidx++] = row_value;
          row_value = 0;
          joffset = 1;
        }
        row_value = (row_value << 1);
        if (*(bptr + j + joffset) != '0') {
          row_value += 1;
        }
      }
      OLED_custom_bmp[bmpidx++] = row_value;
    }
    // for(int i=0; i<h; i++) {
    //   Serial.print(OLED_custom_bmp[i*row_byte_count], HEX);
    //   if (w>8) {
    //     Serial.print(",");
    //     Serial.print(OLED_custom_bmp[i*row_byte_count+1], HEX);
    //   }
    //   Serial.print("\n");
    // }
  }
  else if(command == "drawbmp") {
    // Serial.println(cmd_str);
    // Serial.println(param_count);
    x0 = String(cptr[1]).toInt();
    y0 = String(cptr[2]).toInt();
    w = String(cptr[3]).toInt();
    h = String(cptr[4]).toInt();
    color = (uint16_t) String(cptr[5]).toInt();

    if (w > 16) return -6; // max width 16
    if (h > 16) return -7; // max height 16

    display.drawBitmap(x0, y0, OLED_custom_bmp, w, h, color);
  }
  else { // undefined command
    result = -5;
  }
  display.display(); // MUST call this to udpate the buffer to the display
  return result;
}




// Setup a RotaryEncoder with 4 steps per latch for the 2 signal input pins:
// RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::FOUR3);
  // encoder position 20 per turn for mode : FOUR3
  //   each step output = 1
  //   no delay  = 20
  //   delay(3)  = 20
  //   delay(5)  = 18 (missed 2)
  //   delay(10) = ~10-13 (missed ~10)
// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
// RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
  // encoder position 40 per turn for mode : TWO03
  //   each step output = 2
  //   no delay  = 40
  //   delay(3)  = 40
  //   delay(5)  = 37-40 (missed 1-3)
  //   delay(10) = ~21 (missed ~20)
  // clockwise      : dir =  1
  // anti-clockwise : dir = -1
// #define ROTARYENCODER_PIN1 32 // need pin for declaration - just use 32 & 33 as dummy pins
// #define ROTARYENCODER_PIN2 33 // change to user defined pin on start
// #define ROTARYENCODER_PIN1 17 // need pin for declaration - just use 32 & 33 as dummy pins
// #define ROTARYENCODER_PIN2 23 // change to user defined pin on start
boolean flag_rotaryencoder_run = false;
int rotary_encoder_position = 0; 
int rotary_encoder_aState;
int rotary_encoder_aLastState;  
int rotary_encoder_pinA;
int rotary_encoder_pinB;

void rotary_encoder_init(int pinA, int pinB) {
  rotary_encoder_pinA = pinA;
  rotary_encoder_pinB = pinB;
  pinMode(rotary_encoder_pinA, INPUT);
  pinMode(rotary_encoder_pinB, INPUT);
  // Reads the initial state of the outputA
  rotary_encoder_aLastState = digitalRead(rotary_encoder_pinA);   
}

int rotary_encoder_get_position() {
  return rotary_encoder_position;
}

void rotary_encoder_reset_position() {
  rotary_encoder_position = 0;
}

void rotary_encoder_update() {
  rotary_encoder_aState = digitalRead(rotary_encoder_pinA); // Reads the "current" state of the rotary_encoder_pinA
  // If the previous and the current state of the rotary_encoder_pinA are different, that means a Pulse has occured
  if (rotary_encoder_aState != rotary_encoder_aLastState){     
    // If the rotary_encoder_pinB state is different to the rotary_encoder_pinA state, that means the encoder is rotating clockwise
    if (digitalRead(rotary_encoder_pinB) != rotary_encoder_aState) { 
      rotary_encoder_position ++;
    } else {
      rotary_encoder_position --;
    }
    // Serial.print("Position: ");
    // Serial.println(rotary_encoder_position);
  } 
  rotary_encoder_aLastState = rotary_encoder_aState; // Updates the previous state of the outputA with the current state

}

void rotary_encoder_process_update() {
  if(!flag_rotaryencoder_run) { // rotary encoder not in use - skip
    return;
  }
  rotary_encoder_update();
}

void rotary_encoder_process_cmd(String cmd_str, char * resp_buf) {
  // log_d("i2c_oled_process:%p:%s",cmd_str, cmd_str);
  // Serial.println(cmd_str);
  
  // extract parameters
  char *pch;
  char *cptr[3];
  int param_count = 0;
  pch = strtok((char*)cmd_str.c_str(), ",");
  while (pch != NULL) {
    cptr[param_count++] = pch;
    // log_d("parameter:%p:%s",pch,pch);
    pch = strtok(NULL, ",");
  }
  // log_d("param count=%d",param_count);
  // for (int i=0; i<param_count; i++)
  //   log_d("param_cptr%d=%s", i, cptr[i]);

  String command = String(cptr[0]);
  String resp="";
  if(command == "start") {
    // Serial.println("start");
    int pin1 = String(cptr[1]).toInt();
    int pin2 = String(cptr[2]).toInt();
    // RotaryEncoder encoder(pin1, pin2, RotaryEncoder::LatchMode::FOUR3);
    rotary_encoder_init(pin1, pin2);

    flag_rotaryencoder_run = true;
  }
  else if(command == "status") {
    // Serial.println("status");
    // return position & rotation
    int newPos = rotary_encoder_get_position();
    // int newPos = encoder.getPosition();
    // int newRotation = (int) encoder.getDirection();
    // String resultstr = String(newPos) + "," + String(newRotation) + "\n\n";
    String resultstr = String(newPos) + "\n\n";
    resp = "OK\nROTARY_ENCODER\n" + resultstr;
  }
  else if(command == "reset") {
    // Serial.println("reset");
    // encoder.setPosition(0); // reset position to zero
    rotary_encoder_reset_position();
  }
  else if(command == "stop") {
    // Serial.println("stop");
    flag_rotaryencoder_run = false;
  }

  // Response format is
  // line 1 : OK / NAK
  // line 2 : device/function name
  // line 3 : position
  // line 4 : NULL
  strcpy(resp_buf, resp.c_str());
}
