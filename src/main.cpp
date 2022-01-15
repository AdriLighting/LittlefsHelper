#include <Arduino.h>
#include <Arduino.h>
#if defined(ESP8266)
	#include <LittleFS.h>
#elif defined(ESP32)
	#include <SPIFFS.h>
#else
#endif

#include <ArduinoJson.h>

#include "littlefs_helper.h"

void setup() {
  Serial.begin(115200);

  for(unsigned long const serialBeginTime = millis(); !Serial && (millis() - serialBeginTime > 5000); ) { }
  delay(300);

  Serial.println(F("\n##################################################\n"));  

  Serial.setDebugOutput(false);

  LittleFS.begin();
  LittleFS.format();
  l_1();

/*
  StaticJsonDocument<100> testDocument;
 
  testDocument["sensorType"] = "temperature";
  testDocument["sensorValue"] = 10;
  
  JsonObject documentRoot = testDocument.as<JsonObject>();
 
  for (JsonPair keyValue : documentRoot) {
    Serial.println(keyValue.value().as<String>());
  }*/

}

void loop() {
	
}

