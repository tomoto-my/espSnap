 # include "LittleFS_save_config.h"

/// Uncomment the next line for verbose output over UART.
// #define SERIAL_VERBOSE

/**
   @brief Read WiFi connection information from file system.
   @param ssid String pointer for storing SSID.
   @param pass String pointer for storing PSK.
   @return True or False.

   The config file has to contain the WiFi SSID in the first line
   and the WiFi PSK in the second line.
   Line separator can be \r\n (CR LF) \r or \n.
*/
bool loadConfig(String *ssid, String *pass, String *mdnsid) {
  // open file for reading.
  File configFile = LittleFS.open("/cl_conf.txt", "r");
  if (!configFile) {
    Serial.println("Failed to open cl_conf.txt.");

    return false;
  }

  // Read content from config file.
  String content = configFile.readString();
  configFile.close();

  content.trim();

  // Check if there is a second line available.
  int8_t pos = content.indexOf("\r\n");
  uint8_t le = 2;
  // check for linux and mac line ending.
  if (pos == -1) {
    le = 1;
    pos = content.indexOf("\n");
    if (pos == -1) { pos = content.indexOf("\r"); }
  }

  // If there is no second line: Some information is missing.
  if (pos == -1) {
    #ifdef SERIAL_VERBOSE
      Serial.println("Infvalid content.");
      Serial.println(content);
    #endif SERIAL_VERBOSE

    return false;
  }

#ifdef SERIAL_VERBOSE
  Serial.println("----- file content -----");
  Serial.println(content);
#endif SERIAL_VERBOSE

  // Store SSID and PSK into string vars.
  *ssid = content.substring(0, pos);
  // *pass = content.substring(pos + le);
  content = content.substring(pos + le);

  // Check if there is a third line available.
  pos = content.indexOf("\r\n");
  le = 2;
  // check for linux and mac line ending.
  if (pos == -1) {
    le = 1;
    pos = content.indexOf("\n");
    if (pos == -1) { pos = content.indexOf("\r"); }
  }

  // Store PSK and mDNSID into string vars.
  *pass = content.substring(0, pos);
  *mdnsid = content.substring(pos + le);

  ssid->trim();
  pass->trim();
  mdnsid->trim();

#ifdef SERIAL_VERBOSE
  Serial.println("----- file content -----");
  Serial.println("ssid:   " + *ssid);
  Serial.println("psk:    " + *pass);
  Serial.println("mdnsid: " + *mdnsid);
#endif

  return true;
}  // loadConfig


/**
   @brief Save WiFi SSID and PSK to configuration file.
   @param ssid SSID as string pointer.
   @param pass PSK as string pointer,
   @return True or False.
*/
bool saveConfig(String *ssid, String *pass, String *mdnsid) {
  // Open config file for writing.
  File configFile = LittleFS.open("/cl_conf.txt", "w");
  if (!configFile) {
    Serial.println("Failed to open cl_conf.txt for writing");

    return false;
  }

  // Save SSID and PSK.
  configFile.println(*ssid);
  configFile.println(*pass);
  configFile.println(*mdnsid);

  configFile.close();

  Serial.println("Saved Config in cl_conf.txt");
  return true;
}  // saveConfig


void deleteConfig() {
  Serial.printf("Deleting file: %s\n", "/cl_conf.txt");
  if (LittleFS.remove("/cl_conf.txt")) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}