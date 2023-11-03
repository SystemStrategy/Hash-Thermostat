

char Version[12] = "Dev V1.03";

bool Miner_Offline = false;
int Miner_Status;  //0=Off,1=ON
int Power;
String Miner_IP;
String Miner_Status_str = "Offline";
int Temp_Low = 71;
int Temp_High = 80;
int Min_Off_Time = 1;
int Min_Run_Time = 1;
String Login_Password;

String Wifi_SSID;
String Wifi_Password;

bool mqtt_enabled = false;
String mqtt_server;
int mqtt_server_port = 1883;
String mqtt_username;
String mqtt_password;
String mqtt_base_topic = "Hash_Thermostat";
int mqtt_update_rate = 10;  //seconds



bool last_Miner_Status;
bool Miner_Offline_Now;
bool last_Miner_Offline;
bool last_MQTT_Connected;


bool Wifi_Station_Mode = false;

//NTP Variables
String NTP_Time;
struct tm timeinfo;
String NTP_Server = "pool.ntp.org";
int UTC_Offset;
int EN_DST;
int TimeID = 6;

unsigned long Uptime = 0;
long Miner_Poll;
unsigned long Wifi_Retry_Timer = 0;
unsigned long lastUptime = 0;
unsigned long lastMsg = 0;
unsigned long last_Temp_Measure = 0;
unsigned long lastRun = 0;
unsigned long lastOff = 0;
unsigned long lastMQTT = 0;
unsigned long MQTT_Startup_Delay = 0;
unsigned long Time_Validation = 0;
unsigned long Miner_Offline_Timer = 0;
unsigned long now;

char Stored_Version[12];
char mqtt_server_Val[20];
char mqtt_username_Val[30];
char mqtt_password_Val[30];


#include "Temperature_Sensor.h";
TemperatureSensor tempSensor;

//Wifi Stuff
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "time.h"
#include "ntp_handler.h";
WebServer server(80);  //Server on port 80
#include "file_handler.h"
#include "API.h"

IPAddress ip(10, 0, 0, 10);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);

#include "MQTT_Handler.h";
MQTTManager mqttManager;

String Current_Webpage;
#include "webpages.h"
#include "webpage_handler.h"




void setup_wifi() {
  if (WiFi.status() != WL_CONNECTED) {
    int count = 0;
    int attempts = 0;
    char SSID_Val[20];
    char Pass_Val[30];
    Wifi_SSID.toCharArray(SSID_Val, Wifi_SSID.length() + 1);
    Wifi_Password.toCharArray(Pass_Val, Wifi_Password.length() + 1);
    WiFi.begin(SSID_Val, Pass_Val);
    while (WiFi.status() != WL_CONNECTED) {
      delay(50);
      count++;
      if (count >= 100) {  //after 5 seconds of no connection give up
        delay(10000);
        break;
      }
    }
  }
}

void reconnect_wifi() {
  Serial.println("Wifi Reconnecting");
  WiFi.disconnect();
  yield();
  delay(100);
  setup_wifi();
}




void setup() {
  Serial.begin(115200);
  delay(1000);
  if (!SPIFFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
    Serial.println("LITTLEFS Mount Failed");
    File_Avaliable = false;
    return;
  } else File_Avaliable = true;
  Read_Parameters_File(SPIFFS, "/Parameters.txt");

  mqtt_server.toCharArray(mqtt_server_Val, mqtt_server.length() + 1);
  mqttManager.setServer(mqtt_server_Val, mqtt_server_port);


  mqtt_username.toCharArray(mqtt_username_Val, mqtt_username.length() + 1);
  mqtt_password.toCharArray(mqtt_password_Val, mqtt_password.length() + 1);  
  mqttManager.setcreds(mqtt_username_Val, mqtt_username_Val, mqtt_password_Val);


  if (!Wifi_Station_Mode) {
    WiFi.softAPConfig(ip, gateway, subnet);
    WiFi.mode(WIFI_AP);
    WiFi.softAP("AP_Thermostat", "StackSats!");  //Configuration Access Point
  } else {
    char SSID_Val[20];
    char Pass_Val[30];
    Wifi_SSID.toCharArray(SSID_Val, Wifi_SSID.length() + 1);
    Wifi_Password.toCharArray(Pass_Val, Wifi_Password.length() + 1);

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID_Val, Pass_Val);  //Connect to your WiFi router

    int count = 0;
    int attempts = 1;

    Serial.println("");
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
          count++;
          if (count >= 30) break;
        }
        WiFi.begin(SSID_Val, Pass_Val);  //Connect to your WiFi router
        count = 0;
      }
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("");
      Set_NTP_Server();
      printLocalTime();
      delay(1000);
    }
  }
  MDNS.begin("thermostat");
  Start_Web_Services();
  Create_Cookie_Tolken();
  tempSensor.initialize();


  now = millis();
  lastMsg = now - 5000;

  lastLoginTime = now;
  lastUptime = now;
  lastRun = now;
  lastOff = now;
  lastMQTT = now;
  MQTT_Startup_Delay = now;
  Time_Validation = now;
  Miner_Offline_Timer = now;

  if (strcmp(Stored_Version, Version) != 0) {
    if (strcmp(Stored_Version, "") != 0) Append_Log_File("INFO - Firmware Updated");
    Save_Parameters_File();
  }
  char buf[30];
  sprintf(buf, "INFO - Device Boot ");
  strcat(buf, Version);
  Append_Log_File(buf);




  Serial.println("WiFi.localIP()");
}


void loop() {

  now = millis();
  server.handleClient();  //Handle client requests

  if (now - lastUptime > 1000) {
    lastUptime = now;
    Uptime++;
  }


  //Temperature Sensor Updates
  if (now - last_Temp_Measure > 10000) {
    last_Temp_Measure = now;

    tempSensor.measureTemperature();

    bool last_Temp_Sensor_Error;

    if ((last_Temp_Sensor_Error != tempSensor.hasError()) && tempSensor.hasError()) Append_Log_File("ERROR - Temperature Sensor Offline");
    else if ((last_Temp_Sensor_Error != tempSensor.hasError()) && !tempSensor.hasError()) Append_Log_File("INFO - Temperature Sensor Online");
    last_Temp_Sensor_Error = tempSensor.hasError();
  }

  if (Wifi_Station_Mode) {
    if (WiFi.status() != WL_CONNECTED) {
      reconnect_wifi();
      Append_Log_File("INFO - Wifi Reconnect");
    }

    if (now - lastMsg > (10000 * Miner_Poll)) {
      lastMsg = now;

      if (WiFi.status() == WL_CONNECTED) {
        if (Get_Miner_Status()) Miner_Offline_Now = true;
        else {
          Miner_Offline_Now = false;
          Miner_Offline_Timer = now;
        }


        if ((last_Miner_Offline != Miner_Offline) && Miner_Offline) Append_Log_File("ERROR - Miner Offline");
        else if ((last_Miner_Offline != Miner_Offline) && !Miner_Offline) Append_Log_File("INFO - Miner Online");
        last_Miner_Offline = Miner_Offline;

        if ((last_Miner_Status != Miner_Status) && !Miner_Status) Append_Log_File("INFO - Miner Off");
        else if ((last_Miner_Status != Miner_Status) && Miner_Status) Append_Log_File("INFO - Miner Hashing");
        last_Miner_Status = Miner_Status;



        if (now - Miner_Offline_Timer > 20000) {
          Miner_Offline = true;
          Miner_Poll = 2;
        } else {
          Miner_Poll = 1;
          Miner_Offline = false;
        }


        if (Miner_Status == 0) lastRun = now;
        else if (Miner_Status == 1) lastOff = now;

        if (!tempSensor.hasError()) {
          float Actual_Temp = tempSensor.getTemperature();
          if ((Actual_Temp < Temp_High) && (Actual_Temp > Temp_Low)) Time_Validation = now;

          if (!Miner_Offline_Now) {
            if (now - Time_Validation > (5 * 60000)) {  //5 minute delay before turning on or off to prevent miss-reads or false conditions
              if (now - lastRun > (Min_Run_Time * 60000)) {
                //Allowed to turn Off
                if (Actual_Temp > Temp_High) {
                  if (Miner_Status != 0) {
                    delay(100);
                    Append_Log_File("INFO - Miner Off Command");
                    Miner_Set_Off();
                    yield();
                  }
                }
              }
              yield();

              if (now - lastOff > (Min_Off_Time * 60000)) {
                //Allowed to turn On
                if (Actual_Temp < Temp_Low) {
                  if (Miner_Status != 1) {
                    delay(100);
                    Append_Log_File("INFO - Miner On Command");
                    Miner_Set_On();
                    yield();
                  }
                }
              }
            }
          } else {  // If the temp sensor goes offline for a minute turn the miner off
            if (tempSensor.getlastupdate() > 60000) {
              if (Miner_Status != 0) {
                Append_Log_File("ERROR - Temperature Sensor Failed");
                delay(100);
                Append_Log_File("INFO - Shutting Miner Down");
                Miner_Set_Off();
                yield();
              }
            }
          }
        }
      }
      if (mqtt_enabled) {
        char mqtt_base_topic_Val[30];
        mqtt_base_topic.toCharArray(mqtt_base_topic_Val, mqtt_base_topic.length() + 1);
        mqttManager.publishTempSensorData(mqtt_base_topic_Val, tempSensor.getTemperature(), tempSensor.hasError());
        mqttManager.publishMinerData(mqtt_base_topic_Val, Miner_Offline, Miner_Status, Power);
        mqttManager.publishParameterData(mqtt_base_topic_Val, Uptime, Temp_Low, Temp_High, Min_Off_Time, Min_Run_Time);
      }
    }
    yield();
  }
}
