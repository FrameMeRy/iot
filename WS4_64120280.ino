#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include <DHT.h>


int count = 0;

const char* ssid = "FrameMeRy";
const char* password = "framemery";

unsigned long channelID = 2337009;
const char* writeKey = "CYL4MZLO7R4NR6DT";

unsigned long lastTime = 0;
unsigned long timerDelay = 15000;

WiFiClient client;

DHT dht(D4, DHT11); 

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  dht.begin();
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("Attempting to connect");
      while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, password);
        delay(5000);
      }
      Serial.println("\nConnected.");
    }

    float temperature = dht.readTemperature(); 
    float humidity = dht.readHumidity(); 

    if (!isnan(temperature) && !isnan(humidity)) {
      ThingSpeak.setField(1, humidity);
      ThingSpeak.setField(2, temperature);
      int x = ThingSpeak.writeFields(channelID, writeKey);
      
      if (x == 200) {
          Serial.print("จำนวนครั้งที่: ");
          Serial.println(++count);
          Serial.println("___________________");
          Serial.print("Temperature: ");
          Serial.print(temperature);
          Serial.println(" °C");
          Serial.print("Humidity: ");
          Serial.print(humidity);
          Serial.println(" %");
      } else {
        Serial.println("Problem updating channel. HTTP error code " + String(x));
      }
    } else {
      Serial.println("Failed to read data from the sensor.");
    }

    lastTime = millis();
  }
}