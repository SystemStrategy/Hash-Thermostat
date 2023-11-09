#include <WiFiClient.h>
#include <PubSubClient.h>

class MQTTManager {
public:
  MQTTManager();
  bool checkConnection();
  void LogConnection();
  void publishTempSensorData(char* base_topic, float temperature, bool online);
  void publishMinerData(char* base_topic, int Miner_Offline, int Miner_Status, int Power) ;
  void publishParameterData(char* base_topic, long Uptime, int Temp_Low, int Temp_High, int Min_Off_Time, int Min_Run_Time) ;
  void setcreds(const char* clientID,const char* username,const char* password);
  void setServer(const char* server, int port);

private:
  WiFiClient _wifiClient;
  PubSubClient _mqttClient;
  const char* _server;
  int _port;
  const char* _clientID;
  const char* _username;
  const char* _password;
  unsigned long _lastMQTTAttempt;
};

MQTTManager::MQTTManager()
  : _server(nullptr), _port(0), _clientID(nullptr), _username(nullptr), _password(nullptr), _lastMQTTAttempt(0) {
  // Initialize WiFiClient and PubSubClient here
  _mqttClient.setClient(_wifiClient);
}

void MQTTManager::setServer(const char* server, int port) {
  _server = server;
  _port = port;
}

void MQTTManager::setcreds(const char* clientID,const char* username,const char* password) {
  _clientID = clientID;
  _username = username;
  _password = password;
}

void MQTTManager::LogConnection() {

  bool MQTT_Connected = _mqttClient.connected();
  if ((last_MQTT_Connected != MQTT_Connected) && MQTT_Connected)   Append_Log_File("INFO - MQTT Online");
  else if ((last_MQTT_Connected != MQTT_Connected) && !MQTT_Connected)  Append_Log_File("INFO - MQTT Offline");
  last_MQTT_Connected = MQTT_Connected;
}


bool MQTTManager::checkConnection() {
  LogConnection();

  if (!_mqttClient.connected()) {
      if (millis() - _lastMQTTAttempt > 30000) {
        _lastMQTTAttempt = millis();
        _mqttClient.setServer(_server, _port);
        _mqttClient.connect(_clientID,_username, _password);
        _mqttClient.loop();
      }
    return false;
  }
  return true;
}

void MQTTManager::publishTempSensorData(char* base_topic, float temperature, bool online) {
  if (checkConnection()) {
    char MQTT_Topic_Temp_Sensor[50];
    strcpy(MQTT_Topic_Temp_Sensor, base_topic);
    strcat(MQTT_Topic_Temp_Sensor, "/Temp_Sensor");

    char MSGString1[247];
    DynamicJsonBuffer JsonBuffer1;
    JsonObject& AHT21_Sensor = JsonBuffer1.createObject();
    AHT21_Sensor["Online"] = online;
    if (!tempSensor.hasError()) {
      AHT21_Sensor["Temperature"] = temperature;
    }
    AHT21_Sensor.printTo(MSGString1);
    _mqttClient.publish(MQTT_Topic_Temp_Sensor, MSGString1);
  }
}

void MQTTManager::publishParameterData(char* base_topic, long Uptime, int Temp_Low, int Temp_High, int Min_Off_Time, int Min_Run_Time) {
  if (checkConnection()) {

    char MQTT_Topic_Parameters[50];
    strcpy(MQTT_Topic_Parameters, base_topic);
    strcat(MQTT_Topic_Parameters, "/Parameters");

    char MSGString2[247];
    DynamicJsonBuffer JsonBuffer2;
    JsonObject& Parameters = JsonBuffer2.createObject();
    Parameters["Uptime"] = Uptime;
    Parameters["Temp_Low"] = Temp_Low;
    Parameters["Temp_High"] = Temp_High;
    Parameters["Min_Off_Time"] = Min_Off_Time;
    Parameters["Min_Run_Time"] = Min_Run_Time;
    Parameters.printTo(MSGString2);
    _mqttClient.publish(MQTT_Topic_Parameters, MSGString2);
  }
}

void MQTTManager::publishMinerData(char* base_topic, int Miner_Offline, int Miner_Status, int Power) {
  if (checkConnection()) {

    char MQTT_Topic_Miner[50];
    strcpy(MQTT_Topic_Miner, base_topic);
    strcat(MQTT_Topic_Miner, "/Miner");

    char MSGString3[247];
    DynamicJsonBuffer JsonBuffer3;
    JsonObject& Miner = JsonBuffer3.createObject();
    Miner["Online"] = !Miner_Offline;
    Miner["Status"] = Miner_Status;
    Miner["Power"] = Power;
    Miner.printTo(MSGString3);
    _mqttClient.publish(MQTT_Topic_Miner, MSGString3);
  }
}
