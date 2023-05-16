#include <WiFiUdp.h>
#include <ArduinoOTA.h>

void setup_ota() {

  // Hay que añadir el hash md5 de la contraseña
  ArduinoOTA.setPassword("PASSWORD");
  ArduinoOTA.setPasswordHash("YOUR_PASSWORDS_HASH");
  ArduinoOTA.begin();
  Serial.println("OTA Configurado");
}

void handle_OTA() {
  ArduinoOTA.handle();
}

