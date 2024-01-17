#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char *ssid = "ssid";
const char *password = "password";

const long gmtOffset = 3600; // GMT offset in seconds (same as UTC + 1)

WiFiUDP ntpUDP;
// Setup NTP server
NTPClient timeClient(ntpUDP, "pool.ntp.org", gmtOffset);

// Base URL for EDF API Call
const char *baseUrl = "https://api-commerce.edf.fr/commerce/activet/v1/calendrier-jours-effacement?option=EJP&identifiantConsommateur=src";

// GPIO Setup
const int todayLedPin = 12;
const int tomorrowLedPin = 13;

void fetchAndProcessEjpData() {
  Serial.println("Checking for EJP days...");

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
  snprintf(inferiorLimit, sizeof(inferiorLimit), "%04d-%01d-%01d", currentYear - 1, timeinfo->tm_mon + 1, timeinfo->tm_mday + 1);
  char superiorLimit[11];
  snprintf(superiorLimit, sizeof(superiorLimit), "%04d-%01d-%01d", currentYear, timeinfo->tm_mon + 1, timeinfo->tm_mday + 1);

  // Construct the full API url
  String apiUrl = String(baseUrl) + "&dateApplicationBorneInf=" + inferiorLimit + "&dateApplicationBorneSup=" + superiorLimit;
  Serial.println(apiUrl);
  // Data fetching
  HTTPClient http;
  if (http.begin(apiUrl)) {
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) { // TODO retry a few time if not working here
      String payload = http.getString(); // Formatting
      
      // Parsing payload
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        bool isEjpToday = false;
        bool isEjpTomorrow = false;

        // Access calendar data
        const int calendarLength = doc["content"]["options"][0]["calendrier"].size();
        const char *tomorrowStatus = doc["content"]["options"][0]["calendrier"][calendarLength-1]["statut"];
        const char *todayStatus = doc["content"]["options"][0]["calendrier"][calendarLength-3]["statut"];

        // EJP check
        if (strcmp(tomorrowStatus, "EJP") == 0) {
          isEjpTomorrow = true;
          digitalWrite(tomorrowLedPin, HIGH);
        }
        if (strcmp(todayStatus, "EJP") == 0) {
          isEjpToday = true;
          digitalWrite(todayLedPin, HIGH);
        }

        // Printing values for now, connecting to LED later
        Serial.print("Is EJP Today: ");
        Serial.println(isEjpToday);
        Serial.print("Is EJP Tomorrow: ");
        Serial.println(isEjpTomorrow);

        isEjpToday ? digitalWrite(todayLedPin, HIGH) : digitalWrite(todayLedPin, LOW);
        isEjpTomorrow ? digitalWrite(tomorrowLedPin, HIGH) : digitalWrite(tomorrowLedPin, LOW);
      } else {
        Serial.println("Error parsing JSON");
      }
    } else {
      Serial.println("Error in HTTP request");
    }
    http.end();
  } else {
    Serial.println("Failed to connect to API");
  }

  WiFi.disconnect(true);
}

void setup() {
  Serial.begin(115200);

  // Init led state
  pinMode(todayLedPin, OUTPUT);
  pinMode(tomorrowLedPin, OUTPUT);

  // Initial data retrieval at setup, easier debugging
  fetchAndProcessEjpData();
}

void loop() {
  // loop content
}
