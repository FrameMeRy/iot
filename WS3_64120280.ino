#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

ESP8266WebServer server(80);

const int ledPin = D6;
DHT dht14(D4, DHT11);

void init_wifi(String ssid, String password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup(void) {
  Serial.begin(115200);
  init_wifi("FrameMeRy", "framemery");

  pinMode(ledPin, OUTPUT);

  dht14.begin();

  server.on("/", HTTP_GET, []() {
    float temperature = dht14.readTemperature();
    float humidity = dht14.readHumidity();

    String html = "<html><head><style>";
      html += "body {font-family: Arial, sans-serif;}";
      html += ".container {text-align: center;}";
      html += "h1 {color: #333;}";
      html += "p {margin: 10px 0;}";
      html += ".temperature, .humidity {font-weight: bold;}";
      html += ".on {color: green;}";
      html += ".off {color: red;}";
      html += "</style></head><body><div class='container'>";
      html += "<h1>Web Server</h1>";
      html += "<p style=''>Temperature: " + String(temperature) + " C</p>";
      html += "<p>Humidity: " + String(humidity) + " %</p>";
      html += "<p>LED Status: ";
      if (digitalRead(ledPin) == HIGH) {
        html += "<span class='on' style='background-color: #4CAF50; color :#FFFFFF '>On</span>";
      } else {
        html += "<span class='off'style='background-color: #f44336; color :#FFFFFF '>Off</span>";
      }
      html += "</p>";
      html += "<form action='/TurnOn' method='get'><input type='submit' value='Turn On' style='background-color: #4CAF50;  border: none; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px;'></form>";
      html += "<form action='/TurnOff' method='get'><input type='submit' value='Turn Off' style='background-color: #f44336;  border: none; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px;'></form>";
      html += "</div></body></html>";


      html += "<script>";
      html += "function updateData() {";
      html += "  var xhr = new XMLHttpRequest();";
      html += "  xhr.open('GET', '/data', true);";
      html += "  xhr.onreadystatechange = function() {";
      html += "    if (xhr.readyState == 4 && xhr.status == 200) {";
      html += "      var data = JSON.parse(xhr.responseText);";
      html += "      document.getElementById('temperature').textContent = data.temperature + ' C';";
      html += "      document.getElementById('humidity').textContent = data.humidity + ' %';";
      html += "    }";
      html += "  };";
      html += "  xhr.send();";
      html += "}";
      html += "setInterval(updateData, 500);";
      html += "</script>";


    server.send(200, "text/html", html);
  });

  server.on("/TurnOn", HTTP_GET, []() {
    digitalWrite(ledPin, HIGH);
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  });

  server.on("/TurnOff", HTTP_GET, []() {
    digitalWrite(ledPin, LOW);
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  });

  server.on("/data", HTTP_GET, []() {
    float temperature = dht14.readTemperature();
    float humidity = dht14.readHumidity();

    String data = "{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + "}";
    server.send(200, "application/json", data);
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}