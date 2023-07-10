#include <WiFi.h>
#include <WiFiServer.h>
#include <ArduinoJson.h>
#include <HttpClient.h>
#include <Wire.h>


// Config WiFi
const char* ssid = "QiQi";
const char* password = "UyenLe2401";

#define PIN_FAN 4
#define PIN_PUMP 16
#define PIN_LIGHT 17

// Config REST API
WiFiServer server(80);
float fan, pump, light; // Biến để lưu trữ giá trị của các key


void setup() {
  Serial.begin(115200);
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_PUMP, OUTPUT);
  pinMode(PIN_LIGHT, OUTPUT);
  //WiFi init
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  server.begin();
  Serial.println("Server started");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Handle REST API requests
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected");

    // Read request from client
    String request = client.readStringUntil('\r');
    Serial.println(request);

    // Check requested URL
    if (request.indexOf("/postactutor") != -1) {
      // Respond to client
      String response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\nData received successfully";
      client.print(response);

      // Process received data
      String postData = ""; //String de luu data lay tu giao thuc POST
      while (client.available()) {
        postData += client.readStringUntil('\n');
      }
      Serial.println("Data received: " + postData);

      // Find start position of JSON data
      int jsonStart = postData.indexOf("{");

      // Decode JSON string
      DynamicJsonDocument json(1024);
      DeserializationError error = deserializeJson(json, postData.substring(jsonStart));

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
      } else {
        // Get values from JSON keys and save to variables
        fan = json["fan"].as<String>().toInt();
        pump = json["pump"].as<String>().toInt();
        light = json["light"].as<String>().toInt();

        // Print received values to the serial monitor
        Serial.print("Fan: ");
        Serial.println(fan);
        Serial.print("Pump: ");
        Serial.println(pump);
        Serial.print("Light: ");
        Serial.println(light);
       }
       //Xu ly actutor o day
       if(fan == 1) {
        Serial.println("FAN ON");
        digitalWrite(PIN_FAN, HIGH);
       }
       else {
        Serial.println("FAN OFF");
        digitalWrite(PIN_FAN, LOW);
       }
       if(pump == 1) {
        Serial.println("PUMP ON");
        digitalWrite(PIN_PUMP, HIGH);
       }
       else {
        Serial.println("PUMP OFF");
        digitalWrite(PIN_PUMP, LOW);
       }
       if(light == 1) {
        Serial.println("LIGHT ON");
        digitalWrite(PIN_LIGHT, HIGH);
       }
       else {
        Serial.println("LIGHT OFF");
        digitalWrite(PIN_LIGHT, LOW);
       }
      delay(1000);
      client.stop();
      Serial.println("Client disconnected");
    }
    else {
      // Respond to client with "Invalid URL" error
      String response = "HTTP/1.1 404 Not Found\nContent-Type: text/html\n\nInvalid URL";
      client.print(response);
      delay(10);
      client.stop();
      Serial.println("Client disconnected");
    }
  }
}
