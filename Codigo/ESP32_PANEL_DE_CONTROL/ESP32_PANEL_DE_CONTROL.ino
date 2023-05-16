

#include "declaraciones.h"




void setup() {
  pixels.setBrightness(MAX_BRIGHTNESS);

  Serial.begin(115200);


  wifi_connect();
  delay(500);
  fp_init();
  setup_ota();
  espnow_init();

  pinMode(sirena, OUTPUT);
  pinMode(sens_puerta, INPUT_PULLUP);

  pixels.clear();
  pixels.show();

  Serial.println("Programa Iniciado");
}

void loop() {

  handle_OTA();
  led_update();
  buzzer_update();
  handle_keypad(); /*El array 'code' contiene la contraseña escrita*/
  check_code();    /*para actualizar el bool estado alarma*/
  telegram_update();
  wifi_keep_alive();


  /*Variables locales*/
  static byte count = 0;
  static unsigned long t_antes = 0;
  static bool bool_ = false;
  /*Variables locales*/




  if (isopen == false && estado_sistema >= 2 && digitalRead(sens_puerta) == HIGH) {
    isopen = true;
    isopen_reason = 0;
  }

  if (isopen == true) {

    if (estado_sistema < 2) {
      count = 0;
      bool_ = false;
      isopen = false;
      fp_reader = false;
      digitalWrite(sirena, LOW);
      return;  //salimos de este if sin seguir ejecutando este if
    }



    if (millis() - t_antes >= 200 && count < 80) {  // Esperamos un poco antes de que suene la sirena



      bool_ = !bool_;
      count++;

      if (count < 2)
        set_led_color(250, 0, 0, 0);
      else {
        if (bool_)
          set_led_color(190, 255, 80, 0);
        else
          set_led_color(190, 0, 0, 0);
      }

      if (count == 20) {
        /*capturamos una foto*/
        structure senddata{ 0, false };
        esp_now_send(espcamaddress, (uint8_t*)&senddata, sizeof(senddata));
      } else if (count == 40) {
        //send a mesage
        char mystring[150];
        mystring[0] = '\0';  //strcat empieza a concatenar en el null termination
        strcat(mystring, "\xE2\x9A\xA0 ATENCIÓN \xE2\x9A\xA0\n");
        strcat(mystring, texto_isopen_reason[isopen_reason]);
        myBot.sendTo(CHAT_ID, mystring);
        //enviamos la foto
        structure senddata{ 0, true };  //id=0, x=true
        esp_now_send(espcamaddress, (uint8_t*)&senddata, sizeof(senddata));
      }


      if (count >= 40) {
        if (bool_ == true)
          buzzer_tone(buzzer, 450, 200);
        else
          buzzer_tone(buzzer, 600, 200);
      }


      t_antes = millis();
    }

    /***************/

    if (count >= 80) {

      if (millis() - t_antes >= 800) {
        bool_ = !bool_;

        count++;

        if (bool_)
          set_led_color(100, 255, 0, 0);
        else
          set_led_color(300, 0, 0, 0);

        if (count > 108)
          digitalWrite(sirena, LOW);
        else
          digitalWrite(sirena, bool_);


        t_antes = millis();
      }
    }
  } /*end of if(isopen = true)*/
}



void check_code() {

  /*Variables locales*/
  static char last_key;
  static unsigned long t_antes = 0;
  static int count = 0, prev_count = 0;
  static bool b = false;
  /*Variables locales*/



  if (estado_sistema == 0 && (key == 'A' || key == 'B')) {

    if ((digitalRead(sens_puerta) + sens_array[0].x) != 0)  //si hay alguna puerta abierta
      estado_sistema = -1;
    else
      estado_sistema = 1;

    last_key = key;
    key = '\0'; /*vaciamos la variable char para que el usuario tenga (o pueda forzar) que confirmar la tecla*/
    erase_code();
  }

  if (strcmp(code, pass) == 0 && key == '#' && estado_sistema >= 2) {
    erase_code();
    play_melody(2);
    set_led_color(300, 0, 0, 0);
    fp_reader = false;
    estado_sistema = 0;
  }

  else if (strcmp(code, pass) != 0 && key == '#' && estado_sistema >= 2) {
    erase_code();
    play_melody(0);
    //myBot.sendTo(CHAT_ID, "\xE2\x9A\xA0 ATENCIÓN \xE2\x9A\xA0\nIntento de desactivación detectado");
  }

  /*------------------------------------------------------------------------------------------------*/
  if (estado_sistema == -1) { /*RETARDO de activacion cuando hay algun sensor abierto*/
    if (millis() - t_antes >= 200 && count <= 70) {

      if (count - prev_count >= 4)
        prev_count = count;

      if (count - prev_count == 0) {
        buzzer_tone(buzzer, 1864, 120);
        set_led_color(120, 255, 0, 0);
      } else if (count - prev_count == 1) {
        buzzer_tone(buzzer, 1864, 120);
        set_led_color(120, 0, 0, 0);
      }
      count++;
      t_antes = millis();
    }

    if ((digitalRead(sens_puerta) + sens_array[0].x) == 0) { /*solo si todos los sensores estan cerrados continuamos*/
      set_led_color(300, 0, 0, 0);
      erase_code();
      count = prev_count = 0;
      estado_sistema = 1;
    } else if (count == 70 || key) { /*Timeout o cualquier tecla*/
      set_led_color(200, 0, 0, 0);
      erase_code();
      count = prev_count = 0;
      estado_sistema = 0;
    }
  }

  if (estado_sistema == 1) { /*RETARDO de activacion*/

    if (millis() - t_antes >= 400 && count <= 23) {
      if (count >= 22 && b == false) {
        //set_led_color(300, 0, 0, 0);
      } else {
        b = !b;
        if (b == true)
          set_led_color(300, 0, 255, 0);
        else {
          buzzer_tone(buzzer, 2637, 150);
          set_led_color(300, 0, 0, 0);
        }
      }
      count++;
      t_antes = millis();
    }

    if (count == 23 || key == 'A' || key == 'B') {
      erase_code();
      count = 0;
      b = false;

      if (last_key == 'A')
        estado_sistema = 2;
      else if (last_key == 'B')
        estado_sistema = 3;

      set_led_color(700, 0, 255, 0);
      play_melody(1);
      key = '*'; /*Para que el fp sensor se encienda*/
    } else if (key) {
      erase_code();
      count = 0;
      b = false;
      estado_sistema = 0;
      set_led_color(500, 0, 0, 0);
    }
  }
}


void handle_keypad() {


  static unsigned long t_antes = 0;


  if ((estado_sistema >= 2 && millis() - t_antes <= 20000) || isopen == true)
    fp_reader = true;
  else
    fp_reader = false;

  if (estado_sistema >= 2 && key) { /*retardo luz fp*/
    t_antes = millis();
  }


  /* 'pos' es la posicion del ultimo numero que hay escrito */


  key = keypad_1.getKey();

  if (key) {
    // key = i;  //hacemoisque key sea constante en el tiempo
    if (key == '*') {
      structure senddata{ 0, false };  //id=0, x=true
      esp_now_send(espcamaddress, (uint8_t*)&senddata, sizeof(senddata));
    }

    if (key == '0') {
      structure senddata{ 0, true };  //id=0, x=true
      esp_now_send(espcamaddress, (uint8_t*)&senddata, sizeof(senddata));
    }


    buzzer_tone(buzzer, 4186, 50);
    /*
        if (key == '*') {  //Borrar uno
          code[pos - 1] = 0;
          pos = pos - 1;
          pos = constrain(pos, 0 , 4);
        }
    */
    if (key == '*') {  // Borrar todo el array
      pos = 0;
      for (int i = 0; i <= 3; i++) {
        code[i] = 0;
      }
    }

    if (pos <= 3 && key != '#' && key != '*' && key != 'A' && key != 'B') {
      pos += 1;
      pos = constrain(pos, 0, 4);
      code[pos - 1] = key;
    }
    // Serial.println(key);
  }
}

void erase_code() {
  pos = 0;
  for (int i = 0; i <= 3; i++) {
    code[i] = 0;
  }
}
