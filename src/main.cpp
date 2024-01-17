#include <Arduino.h>
#include <WiFi.h>

const char *ssid = "ssid";
const char *password = "password";

void setup() {
  Serial.begin(115200);

  // Initiate WiFi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000); // Higher in production, rate limiting TODO
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // loop content
}
