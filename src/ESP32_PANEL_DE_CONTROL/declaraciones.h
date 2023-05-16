/////////// Sistema de Alarma IoT - ESP32_PANEL_DE_CONTROL //////////
/*

Código necesario para el panel de control

Miguel Rodríguez 2023
Todos los derechos reservados

*/
////////////////////////////////////////////////////////////////////


///////// INCISO /////////
/*

LA LOGICA DE LAS PUERTAS ES:

1 -> ABIERTO
0 -> CERRADO

*/
//////////////////////////

/*Archivo dedicado a la declaración de las variables globales
que necesitan estar presentes a lo largo de todo el código*/




#include <Keypad.h>
#include <Adafruit_NeoPixel.h>

const byte ROWS = 4;  // four rows
const byte COLS = 4;  // four columns
char Keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 12, 14, 27, 26 };
byte colPins[COLS] = { 25, 33, 32, 13 };

Keypad keypad_1 = Keypad(makeKeymap(Keys), rowPins, colPins, ROWS, COLS);

/*VARIABLES GLOBALES*/
char code[5];
byte pos;
char key;
const char pass[] = "1234";
bool isopen, fp_reader;

//ID de los módulos
/*
  ID 0 -> CENTRAL
  ID 1 -> MOVIMIENTO
  ID 2 -> PUERTA INALÁMBRICA 1
  ID 3 -> ESPCAM
*/
int isopen_reason;

char texto_isopen_reason[3][25] = {
  "Puerta principal abierta",
  "Ventana abierta",
  "Movimiento detectado"
};





int estado_sistema = 0;
int prev_estado_sistema = 0;
/*
 -1 -> retardo de activación (aviso de puerta abierta)
  0 -> standby
  1 -> retardo de activación
  2 -> alarma activada (puertas)
  3 -> alarma activada (todo)
*/


#define buzzer 5
#define led 19
#define sirena 4
#define sens_puerta 21

Adafruit_NeoPixel pixels(1, led, NEO_GRB + NEO_KHZ800);
#define MAX_BRIGHTNESS 125

/*/////////////////// Declaraciones pertenecientes a wifi_config.ino //////////////////////////////*/
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <time.h>
#include <AsyncTelegram2.h>

// Timezone definition
#define MYTZ "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"

WiFiClientSecure client;
AsyncTelegram2 myBot(client);


int64_t CHAT_ID = 123456789;//TU CHATID

const char* BOT_TOKEN = "TU_TOKEN";
TBMessage msg;
/*/////////////////////////////////////////////////////////////////////////////////////////////*/


/*//////////////////////////// Declaraciones pertenecientes a esp_now.ino /////////////////////*/
#include <esp_now.h>
esp_now_peer_info_t peerInfo;
uint8_t espcamaddress[] = { 0xB0, 0xB2, 0x1C, 0x98, 0x5E, 0x54 };
// Structure example to receive data


struct structure {
  int id;
  bool x;
  float volt;
};

/*
  ID 0 -> CENTRAL
  ID 1 -> PUERTA INALÁMBRICA 1
  ID 2 -> MOVIMIENTO
  ID 3 -> ESPCAM + PIR
*/

// Create a structure called data
structure recvdata;

/*
  ID 0 -> PUERTA INALÁMBRICA 1
  ID 1 -> MOVIMIENTO
*/
structure sens_array[2];

/*/////////////////////////////////////////////////////////////////////////////////////////////*/
