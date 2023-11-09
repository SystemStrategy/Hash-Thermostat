bool printLocalTime() {
  if (!getLocalTime(&timeinfo)) {
    NTP_Time = "Failed to obtain time";
    return false;
  }
  char timeStringBuff[50]; //50 chars should be enough
  strftime(timeStringBuff, sizeof(timeStringBuff),"%B %d %Y %H:%M:%S", &timeinfo);
  NTP_Time = String(timeStringBuff);
  return true;
}
void Set_NTP_Server() {
  char ntpServer[30];
  NTP_Server.toCharArray(ntpServer, NTP_Server.length() + 1);
  configTime((UTC_Offset * 3600), (EN_DST * 3600), ntpServer);  // Get the NTP Time
}
