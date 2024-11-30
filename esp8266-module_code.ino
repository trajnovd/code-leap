#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// WiFi 
const char* ssid = "";
const char* password = "";

// API 
const char* apiUsername = "";
const char* apiPassword = "";

// API endpoint
const char* apiUrl = "https://skopje.pulse.eco/rest/current";

const char* targetSensorId = "3d5e32fb-3c41-427f-b6ef-ed19e84026dd"; // Bilo koj sensor

void setup() {
  Serial.begin(9600); 
  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
}

void loop() {
  fetchPm10Data();
  delay(600000); // Fetch sekoi 10 minuti
}

void fetchPm10Data() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); 

    HTTPClient https;
    if (https.begin(client, apiUrl)) {
      https.setAuthorization(apiUsername, apiPassword);
      int httpCode = https.GET(); 

      if (httpCode == HTTP_CODE_OK) {
        WiFiClient* stream = https.getStreamPtr();
        String pm10Objects[100]; // ima 74 pm10 sensori
        int objectCount = 0;     
        String currentObject = "";
        bool isReadingObject = false;

        //Pominuva niz site gi barame
        while (stream->connected() || stream->available()) {
          if (stream->available()) {
            char c = stream->read();

            if (c == '{') {
              isReadingObject = true;
              currentObject = ""; 
            }

            if (isReadingObject) {
              currentObject += c; 
            }

            if (c == '}') {
              isReadingObject = false;

              if (currentObject.indexOf("\"type\":\"pm10\"") != -1) {
                if (objectCount < 100) { 
                  pm10Objects[objectCount++] = currentObject;
                }
              }
            }
          }
        }

        for (int i = 0; i < objectCount; i++) {
          if (pm10Objects[i].indexOf(targetSensorId) != -1) {
          
            DynamicJsonDocument doc(8096); 
            DeserializationError error = deserializeJson(doc, pm10Objects[i]);

            if (!error) {
              const char* value = doc["value"];
              Serial.println(value); 
              return; 
            }
          }
        }
      }
      https.end(); 
    }
  }
}
