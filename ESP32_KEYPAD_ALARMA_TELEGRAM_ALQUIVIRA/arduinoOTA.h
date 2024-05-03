#include <WiFiUdp.h>
#include <ArduinoOTA.h>


int progreso = 0;

void setup_ota()
{
  // Hay que añadir el hash md5 de la contraseña
  // MD5(alquivira) = 493f304848e1f23ef7a499cfc43d8bfb
  ArduinoOTA.setPassword("alquivira");
  ArduinoOTA.setPasswordHash("493f304848e1f23ef7a499cfc43d8bfb");
  ArduinoOTA.begin();
  Serial.println("OTA Configurado");
}
