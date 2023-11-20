#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>


const char* ssid = "FrameMeRy";
const char* password = "framemery";
const char* mqtt_server = "172.20.10.3";

WiFiClient espClient;
PubSubClient client(espClient);
bool isLedOn = false;
DHT dht(D4, DHT11); 
const int ledPin = D6; 
unsigned long lastLoopTime = 0;
const unsigned long loopInterval = 5000;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      Serial.println("sent hello world to dht11");
      
      client.subscribe("led");
      client.publish("dht11", "hello world");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
 

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);  
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  
}
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long currentMillis = millis();
  if (currentMillis - lastLoopTime >= loopInterval) {
      lastLoopTime = currentMillis;
    float temperature = dht.readTemperature(); 
    float humidity = dht.readHumidity(); 
      client.publish("dht11", ("Temperature : " + String(temperature)+" °C").c_str());
      client.publish("dht11", ("Humidity : " + String(humidity)+ " %").c_str());
      delay(5000);
  }
  

}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (String(topic) == "led") {
    if (message == "on") {
      digitalWrite(ledPin, HIGH);
      isLedOn = true;
    } else if (message == "off") {
      digitalWrite(ledPin, LOW);
      isLedOn = false;
    }
    client.publish("led", isLedOn ? "on" : "off");
  }
}

 