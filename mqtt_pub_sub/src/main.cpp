#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "globals.h"
#include "aws_iot_creds_template.h"
#include "mqtt_info.h"

WiFiClientSecure secureClient = WiFiClientSecure();
PubSubClient mqttClient(secureClient);

void callback(char* topic, byte* payload, unsigned int length);
DynamicJsonDocument doc(128);

// Takes in a payload and switch pins. Turns off the relay if a specific message is sent (0/1)
void mqtt_relay_switch(byte* payload, unsigned int switchPin, char* mqttConfirmTopic, char* pumpName){
     
     char on_signal = '1';
     char off_signal = '0';
     const char* on_message = "1";
     const char* off_message = "0";

     //turn the switch on if the payload is '1' and publish to the MQTT server a confirmation message
     if(payload[0] == on_signal){
       digitalWrite(switchPin, LOW);

      doc[pumpName] = 1;
      // Can't do this without breaking. Need to make more memory efficient 
      //  doc['device'] = DEVICE_NAME;
      
      char buffer[128];
      serializeJson(doc, buffer);
      mqttClient.publish(mqttConfirmTopic, buffer);
      doc.clear();
      //  mqttClient.publish(mqttConfirmTopic, on_message);
       }
       

      //turn the switch off if the payload is '0' and publish to the MQTT server a confirmation message
     else if (payload[0] == off_signal){
       digitalWrite(switchPin, HIGH);
       
       doc[pumpName] = 0;
      // Can't do this without breaking. Need to make more memory efficient 
      //  doc['device'] = DEVICE_NAME;

       char buffer[128];
       serializeJson(doc, buffer);
       
       mqttClient.publish(mqttConfirmTopic, buffer);
       doc.clear();
      //  mqttClient.publish(mqttConfirmTopic, off_message);
       }
}

void callback(char* topic, byte* payload, unsigned int length) {

  //convert topic to string to make it easier to work with
  String topicStr = topic; 
  //Print out some debugging info
  Serial.println("Callback update.");
  Serial.print("Topic: ");
  Serial.println(topicStr);
  Serial.print(". Message: ");
  // THIS IS SOME DUMB FUCKING SHIT
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  Serial.println();
  
   if (topicStr == switchTopic1) 
    {
      Serial.println("Subscribed message found");
      mqtt_relay_switch(payload, switchPin1, switchConfirmTopic1, pumpName1);
    }
    if (topicStr == switchTopic2) 
    {
      Serial.println("Subscribed message found");
      mqtt_relay_switch(payload, switchPin2, switchConfirmTopic2, pumpName2);
     }
}

// Uses hand encoded certificates to connect to AWS 
void connectToAWS() {
  
  mqttClient.setServer(AWS_END_POINT, AWS_PORT);
  // Making sure that subscribed messages are caught in the callback.
  mqttClient.setCallback(callback);
  secureClient.setCACert(AWS_PUBLIC_CERT);
  secureClient.setCertificate(AWS_DEVICE_CERT);
  secureClient.setPrivateKey(AWS_PRIVATE_KEY);
  
  Serial.println("Connecting to MQTT....");

  // This can't be random. Needs to match the thing name you created for your device and it's certificates
  mqttClient.connect(DEVICE_NAME);
  mqttClient.subscribe(switchTopic1);
  mqttClient.subscribe(switchTopic2);

  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT....Retry");
    mqttClient.connect(DEVICE_NAME);
    
    mqttClient.subscribe(switchTopic1);
    mqttClient.subscribe(switchTopic2);

    delay(5000);
  }
  Serial.println("MQTT Connected");
}

void setup() {
  pinMode(switchPin1, OUTPUT); // Relay Switch 1
  pinMode(switchPin2, OUTPUT); // Relay Switch 2

  digitalWrite(switchPin1, HIGH);
  digitalWrite(switchPin2, HIGH);
  
  Serial.begin(115200);
  connectToWiFi();
  connectToAWS();
  
}

void loop() {

  mqttClient.loop();
  delay(10); 
}

