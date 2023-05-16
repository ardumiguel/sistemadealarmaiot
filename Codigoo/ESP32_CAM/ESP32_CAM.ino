//////////////// Sistema de Alarma IoT - ESP32_CAM /////////////////
/*

Código necesario para el módulo de cámara y sensor PIR

Miguel Rodríguez 2023
Todos los derechos reservados

*/
////////////////////////////////////////////////////////////////////




#include <WiFi.h>
#include <AsyncTelegram2.h>
#include "esp_camera.h"

#include <WiFiClientSecure.h>
WiFiClientSecure client;

#define ssid "SSID"
#define password "PASSWORD"

IPAddress ip(192, 168, 1, 164);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress dns(192, 168, 1, 1);


// Timezone definition to get time from NTP server
#define MYTZ "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"
const char* token = "TU_TOKEN";
int64_t CHAT_ID = TU_CHAT_ID;



AsyncTelegram2 myBot(client);
TBMessage msg;


#include <esp_now.h>
bool hasrun = true;
int pcount = 0;
unsigned long t_antes;
#define LAMP_PIN 4



// Structure to receive data
struct structure {
  int id;
  bool x;
};

//ID de los módulos
/*
  ID 0 -> CENTRAL
  ID 1 -> MOVIMIENTO
  ID 2 -> PUERTA INALÁMBRICA 1
  ID 3 -> ESPCAM
*/


structure recvdata;

camera_fb_t* fb;

void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t* mac_addr, const uint8_t* incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&recvdata, incomingData, sizeof(recvdata));
  if (recvdata.id == 0 && recvdata.x == true) {
    hasrun = false;
  }
  if (recvdata.id == 0 && recvdata.x == false) {
    takepicture();
  }
}

void init_espnow() {

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("ESPNOW init");
}



void setup() {
  pinMode(15, INPUT);
  pinMode(LAMP_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();

  // Start WiFi connection
  WiFi.mode(WIFI_AP_STA);
  //WiFi.config(ip, gateway, subnet, dns);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LAMP_PIN, HIGH);
    delay(250);
    Serial.print(".");
    digitalWrite(LAMP_PIN, LOW);
    delay(250);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
  Serial.print("WiFi channel: ");
  Serial.println(WiFi.channel());
  // Sync time with NTP
  configTzTime(MYTZ, "time.google.com", "time.windows.com", "pool.ntp.org");
  client.setCACert(telegram_cert);

  // Set the Telegram bot properies
  myBot.setUpdateTime(300000);
  myBot.setTelegramToken(token);

  // Check if all things are ok
  Serial.print("\nTest Telegram connection... ");
  myBot.begin() ? Serial.println("OK") : Serial.println("NO oK");

  init_camera();
  init_espnow();
}
void loop() {

  myBot.getNewMessage(msg);  //Necesitamos mantener abierta la conexion con telegram abierta

  if (hasrun == false) {

    if (fb != NULL) {
      delay(1000);
      Serial.println("Sending picture");
      myBot.sendPhoto(CHAT_ID, fb->buf, fb->len); /*Hay que llamar a telegram desde el loop para que sea efectivo, si lo llamamos desde otra funcion no va*/
      esp_camera_fb_return(fb);
      fb = NULL;
    }

    hasrun = true;
  }

  if (millis() - t_antes == 20000 && fb != NULL) {
    for (int i = 1; i <= 3; i++)
      esp_camera_fb_return(fb);
    fb = NULL;
  }
}
