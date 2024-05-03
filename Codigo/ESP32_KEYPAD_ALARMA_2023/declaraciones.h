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
char keypad_code[5];
byte pos;
char key;
char pass[5] = "1234";
bool isopen;

/*
  ID 0 -> CENTRAL
  ID 1 -> PUERTA INALÁMBRICA 1
  ID 2 -> MOVIMIENTO
  ID 3 -> ESPCAM + PIR
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
 -2 -> registro huella
 -1 -> retardo de activación pero con aviso de puerta abierta
  0 -> standby
  1 -> retardo de activación
  2 -> alarma activada (puertas)
  3 -> alarma activada (todo)
*/


#define buzzer 5
#define led 15
#define sirena 4
#define sens_puerta 21

#define SEND_PIC



#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

RgbColor red(255, 0, 0);
RgbColor green(0, 255, 0);
RgbColor blue(0, 0, 255);
RgbColor black(0, 0, 0);
RgbColor white(255, 255, 255);

NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> pixels(8, led);

struct MyAnimationState {
  RgbColor StartingColor;
  RgbColor EndingColor;
};

MyAnimationState animationState[9];
NeoPixelAnimator animations(9);


#define MAX_BRIGHTNESS 255


/*------------------------------------------------Declaraciones pertenecientes a wifi_config*/
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <time.h>
#include <AsyncTelegram2.h>

// Timezone definition
#define MYTZ "CET-1CEST,M3.5.0,M10.5.0/3"

WiFiClientSecure client;
AsyncTelegram2 myBot(client);


int64_t CHAT_ID_1 = 

int64_t CHAT_ID_2 = 

const char* BOT_TOKEN = 
TBMessage msg;
/*------------------------------------------------------------------------------------------*/


/*-----------------------------------------------Declaraciones pertenecientes a data_storage*/
#include <Preferences.h>
Preferences preferences;
/*------------------------------------------------------------------------------------------*/


/*//////////Declaraciones pertenecientes a esp_now//////////*/
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

/*--------------------------------------------------------------------------------------------*/
