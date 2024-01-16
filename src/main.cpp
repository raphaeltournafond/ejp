#include <Arduino.h>

const int builtInLed = 2;  // GPIO pin for the built-in LED

void setup() {
  Serial.begin(115200); // Default 9600
  pinMode(builtInLed, OUTPUT);
  delay(2000);  // Tempo to open the Serial Monitor
  Serial.println("Hello, ESP32!");
}

void loop() {
  digitalWrite(builtInLed, HIGH);
  Serial.println("Built-in LED ON");
  delay(1000);

  digitalWrite(builtInLed, LOW);
  Serial.println("Built-in LED OFF");
  delay(1000);
}
