/////////// Sistema de Alarma IoT - ESP32_ESPNOW_PIR ///////////////
/*

Código necesario para el módulo inalámbrico de sensor PIR

Miguel Rodríguez 2023
Todos los derechos reservados

*/
////////////////////////////////////////////////////////////////////


#include <esp_now.h>
#include <esp_wifi.h>  // necesaria para la linea 37
#include <WiFi.h>

#include <driver/rtc_io.h>

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = { 0x9C, 0x9C, 0x1F, 0xEA, 0x07, 0x0C };

// Structure example to send data
// Must match the receiver structure


struct structure {
  int id;
  bool x;
  float voltage;
};

structure data = { 2, true, 0 };
byte count = 0;
esp_now_peer_info_t peerInfo;


RTC_DATA_ATTR bool pirstat;

esp_err_t sendstatus;

void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  sendstatus = status;
}




void setup() {

  Serial.begin(115200);

  //Factor de conversión 4.07v -> 2360
  /*Hay que medir analog read antes de iniciar el wifi pq sino no va*/
  data.voltage = (float)analogRead(27) * 4.07 / 2360.00;

  if (pirstat == true) {
    pirstat = false;
    pinMode(25, INPUT);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_25, 1);
    esp_deep_sleep_start();
  }

  // Init Serial Monitor

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(2, WIFI_SECOND_CHAN_NONE);//Reemplaza con tu canal
  esp_wifi_set_promiscuous(false);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);


  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }



  pinMode(25, INPUT);
  pirstat = digitalRead(25);


  do {
    esp_now_send(broadcastAddress, (uint8_t*)&data, sizeof(data));
    count++;
    if (count > 20)
      break;
    delay(250);
  } while (sendstatus != ESP_NOW_SEND_SUCCESS);

  if (digitalRead(25) == HIGH)
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_25, 0);
  else
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_25, 1);






  WiFi.mode(WIFI_OFF);
  esp_deep_sleep_start();
}

void loop() {}
