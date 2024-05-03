#include <Keypad.h>
#include <Adafruit_NeoPixel.h>
#include "tone_buzzer.h"

const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
char Keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {12, 14, 27, 26};
byte colPins[COLS] = {25, 33, 32, 13};

Keypad keypad_1 = Keypad(makeKeymap(Keys), rowPins, colPins, ROWS, COLS);

/*VARIABLES GLOBALES*/
char code[5];
int pos;
char key;
const char pass[] = "1234";
bool estado_alarma = false;

#define buzzer 16
#define led 17
#define sirena 4
#define sens_puerta 22

unsigned long t_antes_1 = 0;
unsigned long t_antes_2 = 0;
bool bool_1 = false; /*Para el retardo de activación de la sirena*/
bool bool_2 = false;
bool bool_3 = false; /*Enviar MQTT una sola vez*/
bool bool_4 = false; /*retardo activacion alarma*/
bool prev_bool_2;
bool isopen;

int count_1 = 0; /*Retardo Activación sirena*/
int count_2 = 0; /*Retardo activacion alarma*/
int count_3 = 0; /*Contador para la sirena*/

int melody_enable[3] = {2093, 2637, 3135};
int melody_disable[3] = {3135, 2637, 2093};

void play_melody(int n)
{
  int count = 0;
  long t_antes = 0;

  if (n == 1)
  {
    while (count < 3)
    {
      buzzer_update();
      if (millis() - t_antes >= 100 && count < 3)
      {
        buzzer_tone(buzzer, melody_enable[count], 75);
        count = count + 1;
        t_antes = millis();
      }
      if (count == 3)
      {
        count = 0;
        return;
      }
    }
  }

  else if (n == 2)
  {
    while (count < 3)
    {
      buzzer_update();
      if (millis() - t_antes >= 100 && count < 3)
      {
        buzzer_tone(buzzer, melody_disable[count], 75);
        count = count + 1;
        t_antes = millis();
      }
      if (count == 3)
      {
        count = 0;
        return;
      }
    }
  }

  else if (n == 0)
  {
    while (count < 3)
    {
      buzzer_update();
      if (millis() - t_antes >= 100 && count < 3)
      {
        buzzer_tone(buzzer, 400, 75);
        count = count + 1;
        t_antes = millis();
      }
      if (count == 3)
      {
        count = 0;
        return;
      }
    }
  }
}

Adafruit_NeoPixel pixels(1, led, NEO_GRB + NEO_KHZ800);
#define MAX_BRIGHTNESS 100

#include "wifi_config.h"
#include "webserver.h"
#include "arduinoOTA.h"
#include "RFID.h"

void setup()
{

  Serial.begin(115200);
  pixels.begin();
  wifi_connect();
  setup_ota();
  setup_server();
  RFID_setup();
  delay(500);

  pinMode(sirena, OUTPUT);
  pinMode(sens_puerta, INPUT_PULLUP);

  /*Secuecncia de rainbow*/
  for (int i = 0; i <= 150; i++)
  {
    if (i <= 50)
    {
      pixels.setBrightness(map(i, 0, 50, 0, 255));
    }
    pixels.setPixelColor(0, pixels.ColorHSV(655 * i));
    pixels.show();
    if (i >= 100)
    {
      pixels.setBrightness(map(i, 100, 150, 255, 0));
    }

    delay(25);
  }

  pixels.clear();
  pixels.show();
  pixels.setBrightness(MAX_BRIGHTNESS);

  play_melody(1);

  Serial.println("Programa Iniciado");
}

void loop()
{
  ArduinoOTA.handle();
  server.handleClient();
  buzzer_update();
  RFID_update();
  handle_keypad(); /*El array 'code' contiene la contraseña escrita*/
  check_code();    /*para actualizar el bool estado alarma*/

  if (estado_alarma == true)
  {
    if (digitalRead(sens_puerta) == HIGH)
    {
      isopen = true;
    }
  }

  else
  {
    isopen = false;
    digitalWrite(sirena, LOW);
    count_1 = 0;
    count_3 = 0;
    bool_3 = false;
    bool_1 = false;
  }

  if (isopen == true)
  {

    if (millis() - t_antes_1 >= 200 && count_1 < 70)
    { // Esperamos un poco antes de que suene la sirena

      bool_1 = !bool_1;
      if (bool_1 == false)
      {
        pixels.setPixelColor(0, 255, 120, 0);
      }
      else
      {
        pixels.setPixelColor(0, 0, 0, 0);
      }
      pixels.show();


      if (count_1 >= 30)
      {
        if (bool_3 == false)
        { // Mandamos el aviso al servidor una sola vez
          bot.sendMessage(CHAT_ID, "La alarma se ha activado");
          bool_3 = true;
        }

        if (bool_1 == true)
        {
          buzzer_tone(buzzer, 600, 200);
        }
        if (bool_1 == false)
        {
          buzzer_tone(buzzer, 450, 200);
        }
      }

      count_1 = count_1 + 1;
      t_antes_1 = millis();
    }
    
    /***************/
    
    if (count_1 >= 70)
    {

      if (millis() - t_antes_1 >= 800)
      {
        bool_1 = !bool_1;

        if (bool_1 == true)
        {
          pixels.setPixelColor(0, 255, 0, 0);
        }
        else
        {
          pixels.setPixelColor(0, 0, 0, 0);
        }
        pixels.show();

        if (count_3 > 38)
        {
          digitalWrite(sirena, LOW);
        }
        else
        {
          digitalWrite(sirena, bool_1);
          count_3 = count_3 + 1;
        }

        t_antes_1 = millis();
      }
    }

  }
}

void check_code()
{

  if (estado_alarma == false && key == 'A')
  {
    bool_2 = !bool_2;
    erase_code();
    play_melody(1);
    pixels.setPixelColor(0, 0, 255, 0);
    pixels.show();
  }

  if (strcmp(code, pass) == 0 && key == '#')
  {

    /*if (estado_alarma == false)// En el caso en el que queramos
      //que se active la alarma mediante codigo
      {
      bool_2 = !bool_2;
      erase_code();
      buzzer_tone(100, 2, 25);
      }*/

    if (estado_alarma == true)
    {
      estado_alarma = !estado_alarma;
      erase_code();

      pixels.setPixelColor(0, 0, 0, 0);
      pixels.show();

      play_melody(2);
    }
  }

  else if (strcmp(code, pass) != 0 && key == '#' && estado_alarma == true)
  {
    erase_code();
    play_melody(0);
  }

  /*************************/

  if (bool_2 != prev_bool_2)
  {

    if (estado_alarma == false)
    { /*false porque ahora despues vamos a ir al estado true*/

      if (millis() - t_antes_1 >= 200 && count_2 < 30)
      { /*RETARDO*/
        bool_4 = !bool_4;

        if (bool_4 == true)
        {
          pixels.setPixelColor(0, 0, 255, 0);
        }
        else
        {
          pixels.setPixelColor(0, 0, 0, 0);
        }
        pixels.show();

        count_2 = count_2 + 1;
        t_antes_1 = millis();
      }

      if (count_2 == 30)
      {
        erase_code();
        count_2 = 0;
        prev_bool_2 = bool_2;
        estado_alarma = !estado_alarma;

        pixels.setPixelColor(0, 0, 255, 0);
        pixels.show();

        buzzer_tone(buzzer, 2637, 500);
      }
    }
  }
}

void erase_code()
{
  pos = 0;
  for (int i = 0; i <= 3; i++)
  {
    code[i] = 0;
  }
}

void handle_keypad()
{

  /* 'pos' es la posicion del ultimo numero que hay escrito*/

  key = keypad_1.getKey();

  if (key)
  {
    buzzer_tone(buzzer, 4186, 50);
    /*
        if (key == '*') {  //Borrar
          code[pos - 1] = 0;
          pos = pos - 1;
          pos = constrain(pos, 0 , 4);
        }
    */
    if (key == '*')
    { // Borrar todo el array
      pos = 0;
      for (int i = 0; i <= 3; i++)
      {
        code[i] = 0;
      }
    }

    if (pos <= 3 && key != '#' && key != '*' && key != 'A' && key != 'B')
    {
      pos = pos + 1;
      pos = constrain(pos, 0, 4);
      code[pos - 1] = key;
    }
    // Serial.println(key);
  }
}
