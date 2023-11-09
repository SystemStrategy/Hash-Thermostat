//File System Stuff
#include "FS.h"
#include <LittleFS.h>
#define SPIFFS LittleFS
//#define FORMAT_SPIFFS_IF_FAILED true
#define FORMAT_LITTLEFS_IF_FAILED true
bool File_Avaliable;
char Clear_Text_Data[512];
char Log_Text_Data[10240];
char Log_Text_Data_New[10240];

#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>  //Version 5

void TimezoneCalc(int TZ) {
  switch (TZ) {
    case 1:
      EN_DST = 0;
      UTC_Offset = -12;
      break;
    case 2:
      EN_DST = 0;
      UTC_Offset = -11;
      break;
    case 3:
      EN_DST = 0;
      UTC_Offset = -10;
      break;
    case 4:
      EN_DST = 1;
      UTC_Offset = -9;
      break;
    case 5:
      EN_DST = 1;
      UTC_Offset = -8;
      break;
    case 6:
      EN_DST = 1;
      UTC_Offset = -8;
      break;
    case 7:
      EN_DST = 0;
      UTC_Offset = -7;
      break;
    case 8:
      EN_DST = 1;
      UTC_Offset = -7;
      break;
    case 9:
      EN_DST = 1;
      UTC_Offset = -7;
      break;
    case 10:
      EN_DST = 0;
      UTC_Offset = -6;
      break;
    case 11:
      EN_DST = 1;
      UTC_Offset = -6;
      break;
    case 12:
      EN_DST = 1;
      UTC_Offset = -6;
      break;
    case 13:
      EN_DST = 0;
      UTC_Offset = -6;
      break;
    case 14:
      EN_DST = 0;
      UTC_Offset = -5;
      break;
    case 15:
      EN_DST = 1;
      UTC_Offset = -5;
      break;
    case 16:
      EN_DST = 1;
      UTC_Offset = -5;
      break;
    case 17:
      EN_DST = 1;
      UTC_Offset = -4;
      break;
    case 18:
      EN_DST = 0;
      UTC_Offset = -4;
      break;
    case 19:
      EN_DST = 0;
      UTC_Offset = -4;
      break;
    case 20:
      EN_DST = 1;
      UTC_Offset = -4;
      break;
    case 21:
      EN_DST = 1;
      UTC_Offset = -3.5;
      break;
    case 22:
      EN_DST = 1;
      UTC_Offset = -3;
      break;
    case 23:
      EN_DST = 0;
      UTC_Offset = -3;
      break;
    case 24:
      EN_DST = 1;
      UTC_Offset = -3;
      break;
    case 25:
      EN_DST = 1;
      UTC_Offset = -3;
      break;
    case 26:
      EN_DST = 1;
      UTC_Offset = -2;
      break;
    case 27:
      EN_DST = 0;
      UTC_Offset = -1;
      break;
    case 28:
      EN_DST = 1;
      UTC_Offset = -1;
      break;
    case 29:
      EN_DST = 0;
      UTC_Offset = 0;
      break;
    case 30:
      EN_DST = 1;
      UTC_Offset = 0;
      break;
    case 31:
      EN_DST = 1;
      UTC_Offset = 1;
      break;
    case 32:
      EN_DST = 1;
      UTC_Offset = 1;
      break;
    case 33:
      EN_DST = 1;
      UTC_Offset = 1;
      break;
    case 34:
      EN_DST = 1;
      UTC_Offset = 1;
      break;
    case 35:
      EN_DST = 1;
      UTC_Offset = 1;
      break;
    case 36:
      EN_DST = 1;
      UTC_Offset = 2;
      break;
    case 37:
      EN_DST = 1;
      UTC_Offset = 2;
      break;
    case 38:
      EN_DST = 1;
      UTC_Offset = 2;
      break;
    case 39:
      EN_DST = 1;
      UTC_Offset = 2;
      break;
    case 40:
      EN_DST = 0;
      UTC_Offset = 2;
      break;
    case 41:
      EN_DST = 1;
      UTC_Offset = 2;
      break;
    case 42:
      EN_DST = 1;
      UTC_Offset = 2;
      break;
    case 43:
      EN_DST = 1;
      UTC_Offset = 2;
      break;
    case 44:
      EN_DST = 1;
      UTC_Offset = 2;
      break;
    case 45:
      EN_DST = 0;
      UTC_Offset = 3;
      break;
    case 46:
      EN_DST = 1;
      UTC_Offset = 3;
      break;
    case 47:
      EN_DST = 0;
      UTC_Offset = 3;
      break;
    case 48:
      EN_DST = 0;
      UTC_Offset = 3;
      break;
    case 49:
      EN_DST = 1;
      UTC_Offset = 3.5;
      break;
    case 50:
      EN_DST = 0;
      UTC_Offset = 4;
      break;
    case 51:
      EN_DST = 1;
      UTC_Offset = 4;
      break;
    case 52:
      EN_DST = 1;
      UTC_Offset = 4;
      break;
    case 53:
      EN_DST = 0;
      UTC_Offset = 4.5;
      break;
    case 54:
      EN_DST = 1;
      UTC_Offset = 5;
      break;
    case 55:
      EN_DST = 0;
      UTC_Offset = 5;
      break;
    case 56:
      EN_DST = 0;
      UTC_Offset = 5.5;
      break;
    case 57:
      EN_DST = 0;
      UTC_Offset = 5.5;
      break;
    case 58:
      EN_DST = 0;
      UTC_Offset = 5.75;
      break;
    case 59:
      EN_DST = 1;
      UTC_Offset = 6;
      break;
    case 60:
      EN_DST = 0;
      UTC_Offset = 6;
      break;
    case 61:
      EN_DST = 0;
      UTC_Offset = 6.5;
      break;
    case 62:
      EN_DST = 0;
      UTC_Offset = 7;
      break;
    case 63:
      EN_DST = 1;
      UTC_Offset = 7;
      break;
    case 64:
      EN_DST = 0;
      UTC_Offset = 8;
      break;
    case 65:
      EN_DST = 0;
      UTC_Offset = 8;
      break;
    case 66:
      EN_DST = 0;
      UTC_Offset = 8;
      break;
    case 67:
      EN_DST = 0;
      UTC_Offset = 8;
      break;
    case 68:
      EN_DST = 0;
      UTC_Offset = 8;
      break;
    case 69:
      EN_DST = 0;
      UTC_Offset = 9;
      break;
    case 70:
      EN_DST = 0;
      UTC_Offset = 9;
      break;
    case 71:
      EN_DST = 1;
      UTC_Offset = 9;
      break;
    case 72:
      EN_DST = 0;
      UTC_Offset = 9.5;
      break;
    case 73:
      EN_DST = 0;
      UTC_Offset = 9.5;
      break;
    case 74:
      EN_DST = 0;
      UTC_Offset = 10;
      break;
    case 75:
      EN_DST = 1;
      UTC_Offset = 10;
      break;
    case 76:
      EN_DST = 1;
      UTC_Offset = 10;
      break;
    case 77:
      EN_DST = 0;
      UTC_Offset = 10;
      break;
    case 78:
      EN_DST = 1;
      UTC_Offset = 10;
      break;
    case 79:
      EN_DST = 1;
      UTC_Offset = 11;
      break;
    case 80:
      EN_DST = 1;
      UTC_Offset = 12;
      break;
    case 81:
      EN_DST = 0;
      UTC_Offset = 12;
      break;
    case 82:
      EN_DST = 0;
      UTC_Offset = 13;
      break;
  }
}

void deleteFile(fs::FS& fs, const char* path) {
  //Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path)) {
    //Serial.println("- file deleted");
  } else {
    //Serial.println("- delete failed");
  }
}


void renameFile(fs::FS& fs, const char* path1, const char* path2) {
  //Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    //Serial.println("File renamed");
  } else {
    //Serial.println("Rename failed");
  }
}
void readFile(fs::FS& fs, const char* path) {
  memset(Log_Text_Data, 0, 10240);
  bool Got_File = false;
  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    return;
  }

  if (file && !file.isDirectory()) {
    ////Serial.println("- read from file:");
    int j = 0;
    while (file.available()) {
      char c = (file.read());
      Log_Text_Data[j] = c;
      Got_File = true;
      j++;
    }
    Log_Text_Data[j] = '\0';
  }

  file.close();
}
void writeFile(fs::FS& fs, const char* path, const char* message, size_t len) {
  //Serial.println("Attempting to write File");
  delay(100);
  static uint8_t buf[512];
  int i = 0;
  //Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    //Serial.println("- failed to open file for writing");
    return;
  }
  while (len) {
    size_t toWrite = len;
    if (toWrite > 512) {
      toWrite = 512;
    }
    memcpy(buf, message + (i * 512), toWrite);
    file.write(buf, toWrite);
    len -= toWrite;
    i++;
  }
  //Serial.println("- file written");
  file.close();
}

void Save_Parameters_File() {
  memset(Clear_Text_Data, 0, 512);
  DynamicJsonBuffer JsonBuffer;
  JsonObject& root = JsonBuffer.createObject();

  JsonArray& Temp = root.createNestedArray("Temp");
  Temp.add(Temp_Low);
  Temp.add(Temp_High);
  Temp.add(Min_Off_Time);
  Temp.add(Min_Run_Time);

  JsonArray& Wifi = root.createNestedArray("Wifi");
  Wifi.add(Wifi_Station_Mode);
  Wifi.add(Wifi_SSID);
  Wifi.add(Wifi_Password);

  JsonArray& Miner = root.createNestedArray("Miner");
  Miner.add(Miner_IP);

  JsonArray& Login = root.createNestedArray("Login");
  Login.add(Login_Password);

  JsonArray& MQTT = root.createNestedArray("MQTT");
  MQTT.add(mqtt_enabled);
  MQTT.add(mqtt_server);
  MQTT.add(mqtt_server_port);
  MQTT.add(mqtt_username);
  MQTT.add(mqtt_password);
  MQTT.add(mqtt_base_topic);
  MQTT.add(mqtt_update_rate);


  JsonArray& NTP = root.createNestedArray("NTP");
  NTP.add(TimeID);
  NTP.add(NTP_Server);

  JsonArray& System = root.createNestedArray("System");
  System.add(Version);

  root.printTo(Clear_Text_Data);
  int len = strlen(Clear_Text_Data);
  deleteFile(SPIFFS, "/Parameters.txt");
  writeFile(SPIFFS, "/Parameters.txt", Clear_Text_Data, len);
}

void Read_Parameters_File(fs::FS& fs, const char* path) {
  memset(Clear_Text_Data, 0, 512);
  bool Got_File = false;
  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    return;
  }

  if (file && !file.isDirectory()) {
    ////Serial.println("- read from file:");
    int j = 0;
    while (file.available()) {
      char c = (file.read());
      Clear_Text_Data[j] = c;
      Got_File = true;
      j++;
    }
    Clear_Text_Data[j] = '\0';
  }

  file.close();

  if (Got_File) {
    DynamicJsonBuffer JsonBuffer;
    JsonObject& root = JsonBuffer.parseObject(Clear_Text_Data);

    Temp_Low = root["Temp"][0].as<int>();
    Temp_High = root["Temp"][1].as<int>();
    Min_Off_Time = root["Temp"][2].as<int>();
    Min_Run_Time = root["Temp"][3].as<int>();

    Wifi_Station_Mode = root["Wifi"][0].as<int>();
    Wifi_SSID = root["Wifi"][1].as<String>();
    Wifi_Password = root["Wifi"][2].as<String>();

    Miner_IP = root["Miner"][0].as<String>();

    Login_Password = root["Login"][0].as<String>();

    mqtt_enabled = root["MQTT"][0].as<int>();
    mqtt_server = root["MQTT"][1].as<String>();
    mqtt_server_port = root["MQTT"][2].as<int>();
    mqtt_username = root["MQTT"][3].as<String>();
    mqtt_password = root["MQTT"][4].as<String>();
    mqtt_base_topic = root["MQTT"][5].as<String>();
    mqtt_update_rate = root["MQTT"][6].as<int>();

    TimeID = root["NTP"][0].as<int>();
    NTP_Server = root["NTP"][1].as<String>();
    TimezoneCalc(TimeID);

    String Stored_Version_Str = root["System"][0].as<String>();
    Stored_Version_Str.toCharArray(Stored_Version, Stored_Version_Str.length() + 1);

  }
}

void SetupFile() {
  if (!SPIFFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
    //Serial.println("LITTLEFS Mount Failed");
    File_Avaliable = false;
    return;
  } else File_Avaliable = true;
  Read_Parameters_File(SPIFFS, "/Parameters.txt");
}




//Log Files
void Append_Log_File(const char* message) {
  
  char timeStringBuff[50];  // 50 chars should be enough
  if(printLocalTime()){  
    strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d  %H:%M:%S", &timeinfo);
  } else sprintf(timeStringBuff, "Unknown Time");

  memset(Log_Text_Data_New, 0, 10240);
  readFile(SPIFFS, "/Log.txt");
  delay(1);
  sprintf(Log_Text_Data_New, timeStringBuff);  //Timestamp data
  strcat(Log_Text_Data_New, " - ");
  strcat(Log_Text_Data_New, message);        //Add new data
  strcat(Log_Text_Data_New, "\n");           //Add line break
  strcat(Log_Text_Data_New, Log_Text_Data);  //Append the old data

  Log_Text_Data_New[8120] = '\0';  // Null-terminate the string

  deleteFile(SPIFFS, "/Log.txt");
  delay(1);

  int len = strlen(Log_Text_Data_New);
  writeFile(SPIFFS, "/Log.txt", Log_Text_Data_New, len);
  delay(1);
}

