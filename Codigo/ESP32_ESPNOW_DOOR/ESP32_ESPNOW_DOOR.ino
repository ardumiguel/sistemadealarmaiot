//////////// Sistema de Alarma IoT - ESP32_ESPNOW_DOOR /////////////
/*

Código necesario para el módulo de sensor inalámbrico de puerta/ventana

Miguel Rodríguez 2023
Todos los derechos reservados

*/
////////////////////////////////////////////////////////////////////



#include <esp_now.h>
#include <esp_wifi.h>  // necesaria para la linea 37
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <driver/rtc_io.h>

uint8_t broadcastAddress[] = { 0x9C, 0x9C, 0x1F, 0xEA, 0x07, 0x0C };

struct structure {
  int id;
  bool x;
  float volt;
};
structure data = { 1, 0, 0 };

esp_now_peer_info_t peerInfo;
esp_now_send_status_t sendstatus;

byte count = 0;

void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  sendstatus = status;
}


void setup() {



  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  //disable brownout detector

  Serial.begin(115200);

  //2415 -> 4.05 factor de conversion
  /*Hay que medir analog read antes de iniciar el wifi pq sino no va*/
  data.volt = (float)analogRead(27) * 4.05 / 2415.00;



  WiFi.mode(WIFI_STA);


  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(11, WIFI_SECOND_CHAN_NONE);//Reemplaza con tu canal
  esp_wifi_set_promiscuous(false);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
  }



  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.encrypt = false;
  esp_now_register_send_cb(OnDataSent);


  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
  }


  pinMode(26, INPUT);

  do {

    data.x = !digitalRead(26);

    esp_now_send(broadcastAddress, (uint8_t*)&data, sizeof(data));
    count++;
    if (count > 30)
      break;
    delay(250);

  } while (sendstatus != ESP_NOW_SEND_SUCCESS);

  if (digitalRead(26) == HIGH) {
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_26, 0);
  }

  else {
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_26, 1); 
    rtc_gpio_pulldown_en(GPIO_NUM_26);
  }


  WiFi.mode(WIFI_OFF);
  esp_deep_sleep_start();
}

void loop() {
}