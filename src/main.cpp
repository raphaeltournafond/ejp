#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>

const char *ssid = "ssid";
const char *password = "password";

const long gmtOffset = 3600; // GMT offset in seconds (same as UTC + 1)

WiFiUDP ntpUDP;
// Setup NTP server
NTPClient timeClient(ntpUDP, "pool.ntp.org", gmtOffset);

// Base URL for EDF API Call
const char *baseUrl = "https://api-commerce.edf.fr/commerce/activet/v1/calendrier-jours-effacement?option=EJP&identifiantConsommateur=src";

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

  // Construct the full API url
  String apiUrl = String(baseUrl) + "&dateApplicationBorneInf=" + inferiorLimit + "&dateApplicationBorneSup=" + superiorLimit;
  Serial.println(apiUrl);
  // Data fetching
  HTTPClient http;
  if (http.begin(apiUrl)) {
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString(); // Formatting
      Serial.println(payload);
      // Process of the payload will go here
    } else {
      Serial.println("Error in HTTP request");
    }
    http.end();
  } else {
    Serial.println("Failed to connect to API");
  }
}

void loop() {
  // loop content
}
