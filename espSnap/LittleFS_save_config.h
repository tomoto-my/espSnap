// adapted from Examples/LittleFS/LittleFS_Timestamp
#include <FS.h>
#include <LittleFS.h> 

#define FORMAT_LITTLEFS_IF_FAILED true

bool loadConfig(String *ssid, String *pass, String *mdnsid);
bool saveConfig(String *ssid, String *pass, String *mdnsid);
void deleteConfig();
