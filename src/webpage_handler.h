unsigned long lastLoginTime;
char Login_Cookie[33];

#include "base64.h"
void generateRandomBytes(unsigned char* buffer, int len) {
  for (int i = 0; i < len; ++i) {
    buffer[i] = random(256);  // Generate random bytes
  }
}
void Create_Cookie_Tolken() {
  unsigned char randomBytes[24];
  generateRandomBytes(randomBytes, 24);

  char base64Buffer[64];  // Buffer to hold the Base64 string
  int base64Length = base64_encode(base64Buffer, (char*)randomBytes, 24);

  // Take the first 32 characters of the Base64 string
  strncpy(Login_Cookie, base64Buffer, 32);
  Login_Cookie[32] = '\0';  // Null-terminate the string
}

//Check if header is present and correct
bool is_authentified() {
  if (server.hasHeader("Cookie")) {
    String cookie = server.header("Cookie");
    //Serial.println(cookie);
    if (cookie.indexOf(Login_Cookie) != -1) {
      long logout_Time = 10;  //minutes
      if (millis() - lastLoginTime > (logout_Time * 60000)) {
        lastLoginTime = now;
        Create_Cookie_Tolken();
        return false;
      }
      lastLoginTime = millis();
      return true;
    }
  }
  //Serial.println("Bad Cookie");
  return false;
}

void handleLogin() {
  String msg;
  if (server.hasHeader("Cookie")) {
    String cookie = server.header("Cookie");
  }
  if (server.hasArg("DISCONNECT")) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "0");
    server.send(301);
    Create_Cookie_Tolken();
    return;
  }
  if (server.hasArg("PASSWORD")) {
    if (server.arg("PASSWORD") == Login_Password) {
      Create_Cookie_Tolken();
      if (Login_Password == "admin") server.sendHeader("Location", "/Change_Login");
      else server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", Login_Cookie);
      server.send(301);
      Serial.println("Log in Successful");
      lastLoginTime = millis();
      return;
    }
    msg = "Wrong password! try again.";
    Append_Log_File("ERROR - Login Attempt Failed");
    Serial.println("Log in Failed");
  }
  String s = Login_page;
  s.replace("@@<style>@@", Web_Style);
  s.replace("@@Login_Error@@", msg);
  server.send(200, "text/html", s);  //Send web page
}

void handleNotFound() {
  String message = "404 webpage does not exist";
  server.send(404, "text/plain", message);
}


void Webpage_Filler(String s) {
  String data_clicked = "true";

  if (WiFi.status() == WL_CONNECTED) {
    s.replace("@@Wifi1@@", "Connected");
  } else s.replace("@@Wifi1@@", "Disconnected");
  s.replace("@@Wifi2@@", WiFi.localIP().toString());
  s.replace("@@Wifi5@@", Wifi_SSID);
  s.replace("@@Wifi7@@", Miner_Status_str);
  s.replace("@@Wifi8@@", Miner_IP);

  if (!tempSensor.hasError()) {
    char buf[10];
    sprintf(buf, "%.2f", tempSensor.getTemperature());
    strcat(buf, "°F");
    s.replace("@@Temp@@", buf);
  } else s.replace("@@Temp@@", "--Measurement Error--");

  s.replace("@@Miner_State@@", Miner_Status_str);

  s.replace("@@Gen1@@", String(Temp_Low));
  s.replace("@@Gen2@@", String(Temp_High));
  s.replace("@@Gen3@@", String(Min_Off_Time));
  s.replace("@@Gen4@@", String(Min_Run_Time));


  if (mqtt_enabled) s.replace("@@MQTT1@@", "selected");
  else s.replace("@@MQTT0@@", "selected");
  s.replace("@@MQTT2@@", mqtt_server);
  s.replace("@@MQTT3@@", String(mqtt_server_port));
  s.replace("@@MQTT4@@", mqtt_username);
  s.replace("@@MQTT6@@", mqtt_base_topic);
  s.replace("@@MQTT7@@", String(mqtt_update_rate));

  if (Wifi_Station_Mode && (WiFi.status() != WL_CONNECTED)) printLocalTime();
  s.replace("@@NTP_Time@@", NTP_Time);
  s.replace("@@NTPserv@@", NTP_Server);
  char buf[16];
  sprintf(buf, "%@@TimeZone%lu@@", TimeID);
  s.replace(buf, "selected");

  s.replace("@@Version@@", Version);


  s.replace("@@<style>@@", Web_Style);
  Current_Webpage = s;
}


void general_prog() {
  Webpage_Filler(General_page);                    //Read HTML contents
  server.send(200, "text/html", Current_Webpage);  //Send web page
}
void MQTT_Page_Prog() {
  Webpage_Filler(MQTT_page);                       //Read HTML contents
  server.send(200, "text/html", Current_Webpage);  //Send web page
}
void network_Prog() {
  Webpage_Filler(Networking_page);                 //Read HTML contents
  server.send(200, "text/html", Current_Webpage);  //Send web page
}
void Conf_Prog() {
  Webpage_Filler(Configuration_page);              //Read HTML contents
  server.send(200, "text/html", Current_Webpage);  //Send web page
}
void Log_Prog() {
  Webpage_Filler(Logging_page);                    //Read HTML contents
  server.send(200, "text/html", Current_Webpage);  //Send web page
}
void Login_Prog() {
  Webpage_Filler(Account_page);                    //Read HTML contents
  server.send(200, "text/html", Current_Webpage);  //Send web page
}
void Set_PWD_prog() {
  if (Login_Password == server.arg("current_password")) {
    Login_Password = server.arg("new_password");
    Save_Parameters_File();
    Serial.println("Successful Passsword Change");
  }
  server.sendHeader("Location", "/login");
  server.sendHeader("Cache-Control", "no-cache");
  server.sendHeader("Set-Cookie", "0");
  server.send(301);
}

void Set_Wifi() {
  Wifi_SSID = server.arg("Wifi_SSID");
  Wifi_Password = server.arg("Wifi_Pass");
  char SSID_Val[20];
  char Pass_Val[30];
  Wifi_SSID.toCharArray(SSID_Val, Wifi_SSID.length() + 1);
  Wifi_Password.toCharArray(Pass_Val, Wifi_Password.length() + 1);
  WiFi.begin(SSID_Val, Pass_Val);  //Connect to your WiFi router
  WiFi.mode(WIFI_AP_STA);
  int count = 0;
  int attempts = 1;
  Serial.print("Attempt : ");
  Serial.println(attempts);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    count++;
    Serial.print(".");
    if (count >= 50) {  //after 3 seconds of no connection give up
      attempts++;
      if (attempts >= 3) {
        break;
      }
      Serial.println("");
      Serial.print("Attempt : ");
      Serial.println(attempts);
      WiFi.disconnect();
      while (WiFi.status() == WL_CONNECTED) {
        delay(100);
        break;
      }
      WiFi.begin(SSID_Val, Pass_Val);  //Connect to your WiFi router
      count = 0;
    }
  }
}

void MQTT_prog() {
  mqtt_enabled = server.arg("MQTT1").toInt();
  mqtt_server = server.arg("MQTT2");
  mqtt_server_port = server.arg("MQTT3").toInt();
  mqtt_username = server.arg("MQTT4");
  mqtt_password = server.arg("MQTT5");
  ;
  mqtt_base_topic = server.arg("MQTT6");
  mqtt_update_rate = server.arg("MQTT7").toInt();

  Save_Parameters_File();
  server.sendHeader("Location", "/MQTT");
  server.send(302, "text/plain", "Updated-- Press Back Button");
}

void therm_prog() {
  Temp_Low = server.arg("Gen1").toInt();
  Temp_High = server.arg("Gen2").toInt();
  Min_Off_Time = server.arg("Gen3").toInt();
  Min_Run_Time = server.arg("Gen4").toInt();
  Save_Parameters_File();

  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Updated-- Press Back Button");
}
void ntp_prog() {
  TimeID = server.arg("TimeZoneSelect").toInt();
  NTP_Server = server.arg("NTPserv");
  TimezoneCalc(TimeID);
  Set_NTP_Server();
  printLocalTime();
  Save_Parameters_File();

  server.sendHeader("Location", "/Network");
  server.send(302, "text/plain", "Updated-- Press Back Button");
}

void wifi_prog() {
  Set_Wifi();  //saves parameters if wifi connects
  if (WiFi.status() == WL_CONNECTED) {
    Wifi_Station_Mode = true;
    Save_Parameters_File();
  }
  server.sendHeader("Location", "/Network");
  server.send(302, "text/plain", "Updated-- Press Back Button");
}
void miner_prog() {
  Miner_IP = server.arg("Miner_IP");
  Get_Miner_Status();
  Save_Parameters_File();
  delay(100);
  server.sendHeader("Location", "/Network");
  server.send(302, "text/plain", "Updated-- Press Back Button");
}
void Command_Miner_On() {
  if (Miner_Status != 1) {
    delay(100);
    Append_Log_File("INFO - Miner On User Command");
    Miner_Set_On();
    yield();
  }

  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Updated-- Press Back Button");
}

void Command_Miner_Off() {
  if (Miner_Status != 0) {
    delay(100);
    Append_Log_File("INFO - Miner Off User Command");
    Miner_Set_Off();
    yield();
  }

  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Updated-- Press Back Button");
}
void handleSystemUpload() {  // upload a new file to the Filing system
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("Update: %s\n", upload.filename.c_str());
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {  //start with max available size
      Update.printError(Serial);
      Append_Log_File("ERROR - OTA Update Failed");
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    /* flashing firmware to ESP*/
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
      Append_Log_File("ERROR - OTA Update Failed");
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) {  //true to set the size to the current progress
      Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      server.sendHeader("Location", "/");
      server.send(302, "text/plain", "Updated-- Press Back Button");
      delay(2000);
      ESP.restart();
    } else {
      Update.printError(Serial);
    }
  }
}
bool Check_Auth() {
  if (!is_authentified()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(302);
    delay(1000);
    return false;
  }
  return true;
}
bool Check_get_Auth() {
  if (!is_authentified()) {
    server.sendHeader("Connection", "close");
    return false;
  }
  return true;
}

void Start_Web_Services() {

  server.on("/login", handleLogin);
  server.onNotFound(handleNotFound);

  //here the list of headers to be recorded
  const char* headerkeys[] = { "User-Agent", "Cookie" };
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);

  server.on("/", []() {
    if (Check_Auth()) {
      general_prog();  //form action is handled here
    }
  });
  server.on("/MQTT", []() {
    if (Check_Auth()) {
      MQTT_Page_Prog();  //form action is handled here
    }
  });
  server.on("/Conf", []() {
    if (Check_Auth()) {
      Conf_Prog();  //form action is handled here
    }
  });

  server.on("/Log", []() {
    if (Check_Auth()) {
      Log_Prog();  //form action is handled here
    }
  });
  server.on("/Logout", []() {
    if (Check_Auth()) {
      server.sendHeader("Location", "/login");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "0");
      server.send(301);
    }
  });
  server.on("/Network", []() {
    if (Check_Auth()) {
      network_Prog();  //form action is handled here
    }
  });
  server.on("/ntp_params", []() {
    if (Check_Auth()) {
      ntp_prog();  //form action is handled here
    }
  });
  server.on("/MQTT_params", []() {
    if (Check_Auth()) {
      MQTT_prog();  //form action is handled here
    }
  });
  server.on("/wifi_params", []() {
    if (Check_Auth()) {
      wifi_prog();  //form action is handled here
    }
  });

  server.on("/Change_Login", []() {
    if (Check_Auth()) {
      Login_Prog();  //form action is handled here
    }
  });
  server.on("/Set_PWD", []() {
    if (Check_Auth()) {
      Set_PWD_prog();  //form action is handled here
    }
  });
  server.on("/miner_params", []() {
    if (Check_Auth()) {
      miner_prog();  //form action is handled here
    }
  });
  server.on("/Miner_On", []() {
    if (Check_Auth()) {
      Command_Miner_On();
    }
  });
  server.on("/Miner_Off", []() {
    if (Check_Auth()) {
      Command_Miner_Off();
    }
  });
  server.on("/thermostat_params", []() {
    if (Check_Auth()) {
      therm_prog();  //form action is handled here
    }
  });
  server.on("/getSensorValue", HTTP_GET, []() {
    if (Check_get_Auth()) {
      // Respond with the current sensor value
      tempSensor.measureTemperature();

      if (!tempSensor.hasError()) {
        char buf[10];
        sprintf(buf, "%.2f", tempSensor.getTemperature());
        strcat(buf, "°F");
        server.send(200, "text/plain", buf);
      } else server.send(200, "text/plain", "--Measurement Error--");
    }
  });
  server.on("/getMinerValue", HTTP_GET, []() {
    if (Check_get_Auth()) {
      server.send(200, "text/plain", Miner_Status_str);
    }
  });
  server.on("/getuptime", HTTP_GET, []() {
    char buf[10];
    sprintf(buf, "%d", Uptime);
    server.send(200, "text/plain", buf);
  });
  server.on("/getLog", HTTP_GET, []() {
    if (Check_get_Auth()) {
      readFile(SPIFFS, "/Log.txt");
      delay(1);
      server.send(200, "text/plain", Log_Text_Data);
    }
  });

  server.on("/local-jquery.js", HTTP_GET, []() {
    if (Check_Auth()) {
      server.send_P(200, "text/javascript", jQueryScript);
    }
  });
  server.on("/reset", []() {
    if (Check_Auth()) {
      deleteFile(SPIFFS, "/Log.txt");
      deleteFile(SPIFFS, "/Parameters.txt");
      server.sendHeader("Location", "/login");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "0");
      server.send(301);
      delay(5000);
      ESP.restart();
    }
  });
  server.on("/reboot", []() {
    if (Check_Auth()) {
      server.sendHeader("Location", "/");
      server.send(302, "text/plain", "Updated-- Press Back Button");
      ESP.restart();
    }
  });

  server.on("/Clear_Log", []() {
    if (Check_Auth()) {
      deleteFile(SPIFFS, "/Log.txt");
      server.sendHeader("Location", "/Log");
      server.send(302, "text/plain", "Updated-- Press Back Button");
    }
  });

  server.on(
    "/update", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.send(200);
    },
    handleSystemUpload);


  server.begin();  //Start server
}
