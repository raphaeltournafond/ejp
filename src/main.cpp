#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char *ssid = "ssid";
const char *password = "password";

const long gmtOffset = 3600; // GMT offset in seconds (same as UTC + 1)

WiFiUDP ntpUDP;
// Setup NTP server
NTPClient timeClient(ntpUDP, "pool.ntp.org", gmtOffset);

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

  // Init and sync time
  timeClient.begin();
  timeClient.update();

  // Get UNIX time
  time_t epochTime = timeClient.getEpochTime();

  // Convert to easily readable struct
  struct tm *timeinfo;
  timeinfo = localtime(&epochTime);

  // Get the current year (add 1900)
  int currentYear = 1900 + timeinfo->tm_year;

  // Formatting to URL params
  char inferiorLimit[11];
  snprintf(inferiorLimit, sizeof(inferiorLimit), "%04d-%02d-%02d", currentYear - 1, timeinfo->tm_mon + 1, timeinfo->tm_mday + 1);
  char superiorLimit[11];
  snprintf(superiorLimit, sizeof(superiorLimit), "%04d-%02d-%02d", currentYear, timeinfo->tm_mon + 1, timeinfo->tm_mday + 1);

  Serial.print("Inferior Limit: ");
  Serial.println(inferiorLimit);

  Serial.print("Superior Limit: ");
  Serial.println(superiorLimit);

  // For testing
  char time[9];
  snprintf(time, sizeof(time), "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
  Serial.print("Time verification: ");
  Serial.println(time);
}

void loop() {
  // loop content
}
