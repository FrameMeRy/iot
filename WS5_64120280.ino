#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <TimeLib.h>

int count = 0;

const char* ssid = "FrameMeRy";
const char* password = "framemery";

unsigned long lastTime = 0;
unsigned long timerDelay = 15000;

WiFiClient client;
HTTPClient http;

DHT dht(D4, DHT11);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  dht.begin();

  configTime(0, 0, "pool.ntp.org", "time.nist.gov"); // Set time using NTP servers
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
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor");
    } else { 
      Serial.print("จำนวนครั้งที่: ");
      Serial.println(++count);
      Serial.println("___________________");
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" °C");
      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.println(" %");

      // Get current date and time
      struct tm timeinfo;
      if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
        return;
      }
      char timestamp[20];
      strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);

      // Create JSON object
      StaticJsonDocument<200> jsonDocument;
      jsonDocument["humidity"] = humidity;
      jsonDocument["temperature"] = temperature;
       jsonDocument["timestamp"] = timestamp;
       Serial.println(timestamp);

      // Serialize JSON data to String
      String jsonData;
      serializeJson(jsonDocument, jsonData);

      // Send HTTP POST request
      http.begin(client, "http://172.20.10.4:5512/iots");
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(jsonData);
      if (httpResponseCode > 0) {
        Serial.println("HTTP Response code: " + String(httpResponseCode));
        String payload = http.getString();
        Serial.println("Returned payload:");
        Serial.println(payload);
      } else {
        Serial.println("Error on sending POST: " + String(httpResponseCode));
      }
      http.end();
    }
    lastTime = millis();
  }
}
