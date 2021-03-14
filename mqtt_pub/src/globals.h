const char* ssid     = "your WIFI name";
const char* password = "your WIFI password";

// Find yours by going to AWS IOT, Settings and select Custom Endpoints
const char* AWS_END_POINT = "account-ats.iot.region.amazonaws.com";

// This won't change. 
const int AWS_PORT = 8883;

// Just moved here to make main.cpp easier to read
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