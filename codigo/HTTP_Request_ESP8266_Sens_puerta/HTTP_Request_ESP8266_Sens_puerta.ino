//FC:F5:C4:19:98:7C

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

HTTPClient http;
WiFiClient client;

const char* ssid = "";
const char* password = "";

const char* url = "";

bool bool_1 = true;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  pinMode(2, OUTPUT);

  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(2, LOW);
    delay(250);
    Serial.print(".");
    digitalWrite(2, HIGH);
    delay(250);
  }

  Serial.println("Connected");

  pinMode(12, INPUT_PULLUP);

  http.begin(client, url);

}

void loop() {

  if (digitalRead(12) == LOW) {
    bool_1 = true;
    delay(100);
  }
  // Send message via ESP-NOW
  if (bool_1 == true && digitalRead(12) == HIGH) {
    Serial.println("ENVIADO");
    digitalWrite(2, LOW);
    http.GET();
    bool_1 = false;
    digitalWrite(2, HIGH);
  }



}
