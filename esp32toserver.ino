#include <WiFi.h>
#include "HTTPClient.h"
#include "ArduinoJson.h"
#include "DHT.h"

String apiKeyValue = "tPmAT5Ab3j7F9";
const char* ssid = "your ssid";
const char* password = "your password";
const int soilPin = 34;
int soilValue = 0;
int soilHumidity = 0;
#define DHTPIN 27
#define DHTTYPE DHT11
String sensorName = "dht11";
String locationDht = "Labo";
String locationhygro = "Potager";
String sensorName2 = "hygro";
DHT dht(DHTPIN, DHTTYPE);
const char* serverName = "http://your_url/post_data.php";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
    delay(2000);
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    soilValue = analogRead(soilPin);
    soilHumidity = map(soilValue, 0, 4095, 0, 100);

    // Format the JSON string
        StaticJsonDocument<300> doc;

    // Add data to the JSON document
    doc["apikey"] = apiKeyValue;
    doc.createNestedArray("sensors");
    // Create a sensor object
    JsonObject sensor1 = doc["sensors"].createNestedObject();
    sensor1["name"] = sensorName;
    sensor1["localisation"] = locationDht;
    JsonObject value1 = sensor1.createNestedObject("value");
    value1["temperature"] = t;
    value1["humidite"] = h;

    JsonObject sensor2 = doc["sensors"].createNestedObject();
    sensor2["name"] = sensorName2;
    sensor2["localisation"] = locationhygro;
    JsonObject value2 = sensor2.createNestedObject("value");
    value2["humidite"] = soilHumidity;

    // Serialize the JSON document to a string
    String json;
    serializeJson(doc, json);

    // Send HTTP POST request
    int httpResponseCode = http.POST(json);
  

    if (httpResponseCode > 0) {
      Serial.printf("Réponse du serveur : %d\n", httpResponseCode);
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.printf("Erreur lors de la requête HTTP : %s\n", http.errorToString(httpResponseCode).c_str());
    }

    // Free resources
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  // Send an HTTP POST request every 30 seconds
  delay(30000);
}
