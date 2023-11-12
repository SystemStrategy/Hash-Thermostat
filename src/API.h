
#include <WiFiClient.h>
WiFiClient client1;

#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>  //Version 5

bool Open_Socket() {
  bool Socket_Open = false;
  char Miner_IP_Val[30];
  Miner_IP.toCharArray(Miner_IP_Val, Miner_IP.length() + 1);
  if (client1.connect(Miner_IP_Val, 4028)) {
    Socket_Open = true;
    return true;
  } else return false;
}
void Close_Socket() {
  return;
}


String body = "";

bool Read_API_JSON() {
  body = "";
  long now = millis();
  bool New_JSON = false;
  int JSON_Layer = 0;
  while (millis() - now < 3000) {
    while (client1.available()) {
      char c = client1.read();

      if (c == 123) {
        JSON_Layer++;
        New_JSON = true;
      }
      if (c == 125) {
        JSON_Layer--;
      }

      if (New_JSON) body = body + c;

      if (New_JSON && JSON_Layer == 0) {
        delay(10);
        return true;
        break;
      }
    }
    if (New_JSON && JSON_Layer == 0) break;
  }
  delay(10);
  return false;
}

bool Send_Request_Pause() {
  bool Command_Accepted;
  String Message;
  DynamicJsonBuffer Write_Params;  //To write to Flash
  JsonObject& Miner_root = Write_Params.createObject();
  Miner_root["command"] = "pause";

  Miner_root.printTo(Message);
  client1.println(Message);

  if (Read_API_JSON()) {
    DynamicJsonBuffer Read_Params;
    JsonObject& Miner_Read = Read_Params.parseObject(body);
    Command_Accepted = Miner_Read["STATUS"]["Code"].as<int>() == 204;
    return Command_Accepted;
  } else return false;
}

bool Send_Request_Resume() {
  bool Command_Accepted;
  String Message;
  DynamicJsonBuffer Write_Params;  //To write to Flash
  JsonObject& Miner_root = Write_Params.createObject();
  Miner_root["command"] = "resume";

  Miner_root.printTo(Message);
  client1.println(Message);

  if (Read_API_JSON()) {
    DynamicJsonBuffer Read_Params;
    JsonObject& Miner_Read = Read_Params.parseObject(body);
    Command_Accepted = Miner_Read["STATUS"]["Code"].as<int>() == 205;
    return Command_Accepted;
  } else return false;
}



bool Send_Request_Tuner() {
  bool Command_Accepted;
  String Message;
  DynamicJsonBuffer Write_Params;  //To write to Flash
  JsonObject& Miner_root = Write_Params.createObject();
  Miner_root["command"] = "tunerstatus";

  Miner_root.printTo(Message);
  client1.println(Message);

  if (Read_API_JSON()) {
    int code;
    DynamicJsonBuffer Read_Params;
    JsonObject& Miner_Read = Read_Params.parseObject(body);
    if (Miner_Read.success()) {
      JsonArray& statusArray = Miner_Read["STATUS"];
      for (JsonArray::iterator it = statusArray.begin(); it != statusArray.end(); ++it) {
        JsonObject& statusObject = *it;
        code = statusObject["Code"];
      }
      JsonArray& statusArray1 = Miner_Read["TUNERSTATUS"];
      for (JsonArray::iterator it = statusArray1.begin(); it != statusArray1.end(); ++it) {
        JsonObject& statusObject = *it;
        Power = statusObject["ApproximateMinerPowerConsumption"];
      }
      Command_Accepted = (code == 203);
      return Command_Accepted;
    }
  } else return false;
}


bool Get_Miner_Status() {
  if (Open_Socket()) {
    if (Send_Request_Tuner()) {
      if (Power > 100) Miner_Status = 1;
      else Miner_Status = 0;

      if (Miner_Status == 1) Miner_Status_str = "Hashing";
      else if (Miner_Status == 0) Miner_Status_str = "Off";
      else Miner_Status_str = "Unknown";
      Close_Socket();
      return false;
    } else {
      Miner_Status_str = "Offline";
      return true;
    }
  } else {
    Close_Socket();
    Miner_Status_str = "Offline";
    return true;
  }
}
bool Miner_Set_Off() {
  Serial.println("Miner Off");
  if (Open_Socket()) {
    return Send_Request_Pause();
  } else {
    Close_Socket();
    return false;
  }
}

bool Miner_Set_On() {
  Serial.println("Miner On");
  if (Open_Socket()) {
    return Send_Request_Resume();
  } else {
    Close_Socket();
    return false;
  }
}
