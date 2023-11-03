#include <Wire.h>
#include <Adafruit_AHTX0.h>
Adafruit_AHTX0 aht;

#include <Adafruit_AHTX0.h>

class TemperatureSensor {
public:
  TemperatureSensor() : Temp_Sensor_Error(false),  last_Temp_Measure(0) {}

  bool initialize() {
    aht.begin();
    return aht.getStatus() != 255;
  }

  bool hasError() const {
    return Temp_Sensor_Error;
  }

  void measureTemperature() {
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);

    if (aht.getStatus() == 255) {
      Temp_Sensor_Error = true;
    } else {
      last_Temp_Measure = millis();
      Temp_Sensor_Error = false;
      Actual_Temp = ((temp.temperature * 9 / 5) + 32);
    }
  }

  float getTemperature() const {
    return Actual_Temp;
  }
   unsigned long getlastupdate() const {
    return millis()-last_Temp_Measure;
  }


private:
  Adafruit_AHTX0 aht;
  float Actual_Temp;
  bool Temp_Sensor_Error;
  unsigned long last_Temp_Measure;
};
