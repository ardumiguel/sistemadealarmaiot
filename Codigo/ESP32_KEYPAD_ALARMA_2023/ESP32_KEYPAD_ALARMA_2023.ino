#include "declaraciones.h"

/*/////////////////////
  INCISO

  LA LOGICA DE LAS PUERTAS ES:

  1 -> ABIERTO
  0 -> CERRADO

  /////////////////////*/


void setup() {
  Serial.begin(115200);
  pixels.Begin();
  pixels.Show();

  wifi_connect();
  delay(500);
  ota_init();
  espnow_init();
  data_init();
  RFID_setup();

  pinMode(sirena, OUTPUT);
  pinMode(sens_puerta, INPUT_PULLUP);



  Serial.println("Programa Iniciado");
}

void loop() {

  led_update();
  handle_keypad(); /*El array 'code' contiene la contraseña escrita*/
  check_code();    /*para actualizar el estado alarma*/
  store_data();
  handle_OTA();
  buzzer_update();
  wifi_keep_alive();
  RFID_update();

  telegram_update();


  /*Variables locales*/
  static byte count = 0;
  static unsigned long t_antes = 0;
  static bool bool_ = false;
  /*Variables locales*/

  if (estado_sistema >= 2 && isopen == false) {
    if (millis() - t_antes >= 1500) {

      bool_ = !bool_;
      if (bool_)
        set_led_color(4, 1500, 50, 50, 50);
      else
        set_led_color(4, 1500, 0, 0, 0);

      t_antes = millis();
    }
  }


  if (isopen == false && estado_sistema >= 2 && digitalRead(sens_puerta) == HIGH) {
    isopen = true;
    isopen_reason = 0;
  }

  if (isopen == true) {

    if (estado_sistema < 2) {
      count = 0;
      bool_ = false;
      isopen = false;
      digitalWrite(sirena, LOW);
      return;  //salimos de este if sin seguir ejecutando este if
    }

    if (count == 0)
      for (int i = 0; i < 8; i++)
        set_led_color(i, 1000, 255, 150, 55);



    if (millis() - t_antes >= 200 && count < 80) {  // Esperamos un poco antes de que suene la sirena

      bool_ = !bool_;
      count++;

#ifdef SEND_PIC
      if (count == 19) {
        /*capturamos una foto*/
        structure senddata{ 0, false };
        esp_now_send(espcamaddress, (uint8_t*)&senddata, sizeof(senddata));
      }
#endif

       if (count == 21) {
        myBot.getNewMessage(msg);  //por si a caso no responde
        //send a mesage
        char mystring[150];
        mystring[0] = '\0';  //strcat empieza a concatenar en el null termination
        strcat(mystring, "\xE2\x9A\xA0 ATENCIÓN \xE2\x9A\xA0\n");
        strcat(mystring, texto_isopen_reason[isopen_reason]);
        myBot.sendTo(CHAT_ID_1, mystring);
        myBot.sendTo(CHAT_ID_2, mystring);


        //enviamos la foto

#ifdef SEND_PIC
        structure senddata{ 0, true };  //id=0, x=true
        esp_now_send(espcamaddress, (uint8_t*)&senddata, sizeof(senddata));
#endif
      }


      if (count >= 20) {
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
          for (int i = 0; i < 8; i++)
            set_led_color(i, 300, 255, 0, 0);
        else
          for (int i = 0; i < 8; i++)
            set_led_color(i, 300, 0, 0, 0);

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
    key = '\0'; /*vaciamos la variable char para que el usuario tenga que confirmar la tecla*/
    erase_code();
  }

  if (strcmp(keypad_code, pass) == 0 && key == '#' && estado_sistema >= 2) {
    erase_code();
    play_melody(2);
    for (int i = 0; i < 8; i++)
      set_led_color(i, 300, 0, 0, 0);
    estado_sistema = 0;
  }

  else if (strcmp(keypad_code, pass) != 0 && key == '#' && estado_sistema >= 2) {
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
        for (int i = 0; i < 8; i++)
          set_led_color(i, 200, 255, 0, 0);
      } else if (count - prev_count == 1) {
        buzzer_tone(buzzer, 1864, 120);
        for (int i = 0; i < 8; i++)
          set_led_color(i, 200, 0, 0, 0);
      }

      if (digitalRead(sens_puerta) + sens_array[0].x == 0) {  //solo si todos los sensores estan cerrados continuamos
        for (int i = 0; i < 8; i++)
          set_led_color(i, 300, 0, 0, 0);
        count = prev_count = 0;
        estado_sistema = 1;
      }
      count++;
      t_antes = millis();
    }
    if (key || count == 70) { /*Timeout o cualquier tecla*/
      for (int i = 0; i < 8; i++)
        set_led_color(i, 300, 0, 0, 0);
      count = prev_count = 0;
      estado_sistema = 0;
    }
  }


//////////////////////////////////////////////////////////////////////////////////


  if (estado_sistema == 1) { /*RETARDO de activacion*/

    if (millis() - t_antes >= 400 && count <= 23) {

        b = !b;
        if (b == true)
          for (int i = 0; i < 8; i++)
            set_led_color(i, 300, 0, 255, 0);
        else {
          buzzer_tone(buzzer, 2637, 150);
          for (int i = 0; i < 8; i++)
            set_led_color(i, 300, 0, 0, 0);
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

      for (int i = 0; i < 8; i++)
        set_led_color(i, 1000, 0, 0, 0);
      play_melody(1);
      //key = '*'; /*Para que el fp sensor se encienda*/
    } else if (key) {
      erase_code();
      count = 0;
      b = false;
      estado_sistema = 0;
      for (int i = 0; i < 8; i++)
        set_led_color(i, 500, 0, 0, 0);
    }
  }
}


void handle_keypad() {


  static unsigned long t_antes = 0;




  /* 'pos' es la posicion del ultimo numero que hay escrito */


  key = keypad_1.getKey();

  if (key) {
    // key = i;  //hacemoisque key sea constante en el tiempo

    buzzer_tone(buzzer, 4186, 50);
    /*
        if (key == '*') {  //Borrar uno
          keypad_code[pos - 1] = 0;
          pos = pos - 1;
          pos = constrain(pos, 0 , 4);
        }
    */
    if (key == '*') {  // Borrar todo el array
      pos = 0;
      for (int i = 0; i <= 3; i++) {
        keypad_code[i] = 0;
      }
    }

    if (pos <= 3 && key != '#' && key != '*' && key != 'A' && key != 'B') {
      pos += 1;
      pos = constrain(pos, 0, 4);
      keypad_code[pos - 1] = key;
    }
    Serial.println(key);
  }
}

void erase_code() {
  pos = 0;
  for (int i = 0; i <= 3; i++) {
    keypad_code[i] = 0;
  }
}
