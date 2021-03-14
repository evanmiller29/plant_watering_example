#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

#include "globals.h"
#include "aws_iot_creds_template.h"
#include "mqtt_info.h"

WiFiClientSecure secureClient = WiFiClientSecure();
PubSubClient mqttClient(secureClient);

DynamicJsonDocument doc(256);
#define DHTPIN 14
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    // we can even make the ESP32 to sleep
  }
  Serial.print("Connected - ");
 
}

void connectToAWS() {
  
  mqttClient.setServer(AWS_END_POINT, AWS_PORT);
  // Making sure that subscribed messages are caught in the callback.
  secureClient.setCACert(AWS_PUBLIC_CERT);
  secureClient.setCertificate(AWS_DEVICE_CERT);
  secureClient.setPrivateKey(AWS_PRIVATE_KEY);
  
  Serial.println("Connecting to MQTT....");

  // This can't be random. Needs to match the thing name you created for your device and it's certificates
  mqttClient.connect(DEVICE_NAME);

  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT....Retry");
    mqttClient.connect(DEVICE_NAME);

    delay(5000);
  }
  Serial.println("MQTT Connected");
}

void setup() {

  Serial.begin(115200);
  connectToWiFi();
  connectToAWS();
  dht.begin();
  
}

void loop() {
  // Wait a few seconds between measurements.
  //  Anything later than 15 seconds between iterations stops the MQTT message from sending
  delay(15000);

  // TEST CASES. JUST TO CHECK WHAT HAPPENS
  // mqttClient.publish(weatherStationTopic, "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}");
  // mqttClient.publish(weatherStationTopic67, "HELLO");

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(F(" Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.println();

  // Loading up JSON doc
  doc["humidity"] = h;
  doc["temperature"] = t;
  doc["heat_index"] = hic;

  // For some reason I don't understand putting the JSON to string makes it easier to be ingested on AWS IOT
  String output = "";
  serializeJson(doc, output);
  Serial.println(output);
  mqttClient.publish(weatherStationTopic1, output.c_str());
  // Sending a test packet to see if that works. Looks a little different to the buffer..

  Serial.println("MQTT message sent");

  doc.clear();
}
