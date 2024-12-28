#include <Adafruit_MAX31865.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "TP-Link_BD81";
const char* password = "42844976";

// MQTT broker details
const char* mqtt_server = "192.168.0.101";
const int mqtt_port = 1488;
const char* mqtt_user = "";
const char* mqtt_password = "";

// Sensors IDs
const String sensor_id = "3";

// MQTT topics
const char* mqtt_topic_publish = "mqtt/api/measure";
const char* mqtt_topic_error = "mqtt/api/error";

// Wi-Fi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

void connectWiFi();
void connectMQTT();
void callback(char* topic, byte* message, unsigned int length);

// MAX31865 pins
#define MAX_CS   D8 // Chip Select
#define MAX_MOSI D7 // Master Out Slave In
#define MAX_MISO D6 // Master In Slave Out
#define MAX_CLK  D5 // Clock

// Create an instance of the sensor (RTD type, wires)
Adafruit_MAX31865 max31865 = Adafruit_MAX31865(MAX_CS, MAX_MOSI, MAX_MISO, MAX_CLK);

const float MIN_TEMPERATURE = -50.0; 
const float MAX_TEMPERATURE = 150.0;

void setup() {
  Serial.begin(9600);
  
  connectWiFi();

  // Setup MQTT client
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  connectMQTT();

  // Initialize the MAX31865 (2-wire PT100)
  max31865.begin(MAX31865_2WIRE);
}

void loop() {
  float temperature = max31865.temperature(100.0, 430.0); // 100.0: PT100; 430: Reference resistance
  
  if (temperature < MIN_TEMPERATURE || temperature > MAX_TEMPERATURE) {
    Serial.print("Temperature out of range: ");
    Serial.print(temperature);
    Serial.println(" °C");
    
    if (!client.connected()) {
      connectMQTT();
    }
    client.loop();

    String jsonPayload = "{";
    jsonPayload += "\"message\": \"Invalid sensor data\",";
    jsonPayload += "\"sensor_id\":" + sensor_id + ",";
    jsonPayload += "\"data\":" + String(temperature, 2);
    jsonPayload += "}";

    client.publish(mqtt_topic_error, jsonPayload.c_str());
    Serial.println("Published: " + jsonPayload);
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    if (!client.connected()) {
      connectMQTT();
    }
    client.loop();

    static unsigned long lastMsgTime = 0;
    if (millis() - lastMsgTime > 600000) {
      lastMsgTime = millis();

      String jsonPayload = "{";
      jsonPayload += "\"sensor_id\":" + sensor_id + ",";
      jsonPayload += "\"data\":" + String(temperature, 2);
      jsonPayload += "}";

      client.publish(mqtt_topic_publish, jsonPayload.c_str());
      Serial.println("Published: " + jsonPayload);
    }
  }

  delay(100000);
}

void connectWiFi() {
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected. IP address: " + WiFi.localIP().toString());
}

void connectMQTT() {
  Serial.print("Connecting to MQTT broker...");
  while (!client.connected()) {
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected.");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds.");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  String messageTemp;
  for (unsigned int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  Serial.println("Message: " + messageTemp);
}
