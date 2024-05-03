#include <ArduinoOTA.h>

void ota_init() {
  ArduinoOTA.setHostname("ESP32-PANELCONTROL");
  ArduinoOTA.setRebootOnSuccess(false);


  // Hay que añadir el hash md5 de la contraseña
  ArduinoOTA.setPassword("miguel");
  //ArduinoOTA.setPasswordHash("9eb0c9605dc81a68731f61b3e0838937");
  ArduinoOTA.begin();
  Serial.println("OTA Configurado");

  ArduinoOTA.onEnd([]() {
    myBot.sendTo(CHAT_ID_1, "Actualización OTA correctamente en el ESP32-PANELCONTROL");
    ESP.restart();
  });
}

void handle_OTA() {
  ArduinoOTA.handle();
}
