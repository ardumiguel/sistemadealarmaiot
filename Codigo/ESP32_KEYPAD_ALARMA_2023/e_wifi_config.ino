

const char* ssid = 
const char* password = 


IPAddress ip(192, 168, 1, 25);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 1);

InlineKeyboard kbd_1;
InlineKeyboard kbd_2;

char texto_estado_sistema[4][31] = {
  "Desactivada \xF0\x9F\x94\xB4",
  "Activando",
  "Modo puertas \xF0\x9F\x9B\xA1",
  "Modo completo \xF0\x9F\x9B\xA1"
};
char texto_estado_puerta[2][8] = {
  "Cerrada",
  "Abierta"
};



void wifi_connect() {

  static int count;

  WiFi.setHostname("ESP32-PANELCONTROL");

  WiFi.mode(WIFI_AP_STA);

  WiFi.onEvent(telegram_reset, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);  // cuando se conecte de nuevo reset a la conexion

  WiFi.config(ip, gateway, subnet, dns);
  WiFi.begin(ssid, password);
  WiFi.setAutoReconnect(true);
  WiFi.setAutoConnect(true);


  Serial.println("Connecting wifi");

  Serial.println("");


  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");

    pixels.SetPixelColor(0, RgbColor(90, 0, 255));
    pixels.Show();
    delay(125);
    pixels.SetPixelColor(0, RgbColor(0, 0, 0));
    pixels.Show();
    delay(125);

    count++;
    if (count > 50) {
      count = 0;
      break;
    }
  }
  Serial.println("");




  /*Casi lo mismo que WiFi.printDiag(Serial)*/
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wifi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Channel: ");
    Serial.println(WiFi.channel());
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.println("");
  } else
    Serial.println("Wifi not connected");


  /*TELEGRAM - Lo configuramos sólo cuando se ejecuta por primera vez*/

  configTzTime(MYTZ, "time.google.com", "time.windows.com", "pool.ntp.org");
  client.setCACert(telegram_cert);
  myBot.setUpdateTime(2000);
  myBot.setTelegramToken(BOT_TOKEN);


  kbd_1.addButton("Solo puertas", "puertas", KeyboardButtonQuery);
  kbd_1.addButton("Completo", "completo", KeyboardButtonQuery);

  kbd_2.addButton("Sí", "si", KeyboardButtonQuery);
  kbd_2.addButton("No", "no", KeyboardButtonQuery);

  if (myBot.begin())
    Serial.println("Conexion con telegram establecida");
}


int count;

void telegram_reset(WiFiEvent_t event, WiFiEventInfo_t info) {
  myBot.reset();
  Serial.println("bot reset");
  count++;
}

void wifi_keep_alive() {

  static wl_status_t last_wifi_stat;

  if (WiFi.status() != last_wifi_stat) {

    last_wifi_stat = WiFi.status();

    Serial.println(last_wifi_stat);

    if (last_wifi_stat == WL_DISCONNECTED) { /*En caso de que le de al ESP por desconectrse completamente del wifi y autoreconnect no funcione*/
      WiFi.reconnect();
      count += 1000;
    }
  }
}




void telegram_update() {

  static unsigned long t_antes = 0;
  static bool nextmsg, matches;


  if (millis() - t_antes == 60000) {  //para evitar hacer poll al servidor muy continua cuando no hay actividad reciente
    myBot.setUpdateTime(10000);
    nextmsg = matches = false;
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (myBot.getNewMessage(msg)) {
      if (msg.chatId == CHAT_ID_1 || CHAT_ID_2) {

        myBot.setUpdateTime(2000);
        t_antes = millis();

        //Messagequery//
        if (msg.messageType == MessageQuery) {
          if (msg.callbackQueryData.equals("puertas")) {
            myBot.endQuery(msg, "Alarma activada en el modo Solo puertas");
            myBot.sendMessage(msg, "Alarma activada en el modo Solo puertas");
            estado_sistema = 2;
            erase_code();
            play_melody(1);

          } else if (msg.callbackQueryData.equals("completo")) {
            myBot.endQuery(msg, "Alarma activada en el modo Completo");
            myBot.sendMessage(msg, "Alarma activada en el modo Completo");
            estado_sistema = 3;
            erase_code();
            play_melody(1);

          } else if (msg.callbackQueryData.equals("si")) {
            myBot.endQuery(msg, "Selecciona un modo:");
            myBot.sendMessage(msg, "Selecciona un modo:", kbd_1);

          } else if (msg.callbackQueryData.equals("no")) {
            myBot.endQuery(msg, "Recibido");
            myBot.sendMessage(msg, "Recibido");
          }
        }  //Messagequery//


        if (msg.text == "/log") {
          myBot.sendMessage(msg, (String)count);
        }

        if (msg.text == "/disconnect") {
          WiFi.disconnect();
        }


        if (msg.text == "/contra") {
          myBot.sendMessage(msg, "Introduce la contraseña actual para cambiarla");
          nextmsg = true;
          return;
        }

        else if (nextmsg == true) {

          const char* recv_pass = msg.text.c_str();
          char recv_pass_array[5];
          strncpy(recv_pass_array, recv_pass, 5);

          if (isDigit(recv_pass_array[0]) == false) {
            nextmsg = matches = false;
          } else if (strlen(recv_pass) > 4) {
            myBot.sendMessage(msg, "Contraseña demasiado larga\nIntroduce una contraseña de 4 dígitos");
            return;
          } else {

            if (matches == true) {
              strncpy(pass, recv_pass_array, 5);
              preferences.putChar("storedpass0", recv_pass_array[0]);
              preferences.putChar("storedpass1", recv_pass_array[1]);
              preferences.putChar("storedpass2", recv_pass_array[2]);
              preferences.putChar("storedpass3", recv_pass_array[3]);
              myBot.sendMessage(msg, "Contraseña actualizada");
              nextmsg = false;
              matches = false;
              return;
            }

            if (strcmp(recv_pass_array, pass) == 0) {
              myBot.sendMessage(msg, "Introduce la contraseña nueva");
              matches = true;
            } else
              myBot.sendMessage(msg, "La contraseña no coincide, vuelve a intentarlo");
          }
        }



#ifdef SEND_PIC 
        if (msg.text == "/foto") {
          structure senddata{ 0, false, 0 };  //id=0, x=true
          esp_now_send(espcamaddress, (uint8_t*)&senddata, sizeof(senddata));
          senddata = { 0, true, 0 };  //id=0, x=true
          esp_now_send(espcamaddress, (uint8_t*)&senddata, sizeof(senddata));
        }
#endif

        if (msg.text == "/desactivaralarma") {
          if (estado_sistema == 0) {
            myBot.sendMessage(msg, "La alarma ya está desactivada");
          }

          else {
            estado_sistema = 0;
            erase_code();
            play_melody(2);
            for (int i = 0; i < 8; i++)
              set_led_color(i, 300, 0, 0, 0);

            myBot.sendMessage(msg, "Alarma desactivada correctamente");
          }
        }

        if (msg.text == "/activaralarma") {

          if (estado_sistema >= 2)

            myBot.sendMessage(msg, "La alarma ya está activada");
          else if ((digitalRead(sens_puerta) + sens_array[0].x) != 0) {
            myBot.sendMessage(msg, "Hay alguna puerta o ventana abierta\n¿Desea continuar?", kbd_2);
          } else {
            myBot.sendMessage(msg, "Selecciona un modo:", kbd_1);
          }
        }

        if (msg.text == "/informacion") {
          char mystring[180];

          char volt1[5];
          char volt2[5];
          dtostrf(sens_array[0].volt, 1, 2, volt1);
          dtostrf(sens_array[1].volt, 1, 2, volt2);

          mystring[0] = '\0';  //strcat empieza a concatenar en el null termination
          strcat(mystring, "\xF0\x9F\x8F\xA0 INFORMACIÓN DEL SISTEMA \xF0\x9F\x8F\xA0 \n\n");
          strcat(mystring, "Estado de la alarma: ");
          if (isopen == false)
            strcat(mystring, texto_estado_sistema[estado_sistema]);
          else
            strcat(mystring, texto_isopen_reason[isopen_reason]);
          strcat(mystring, "\n\n");
          strcat(mystring, "Sensores:\n");
          strcat(mystring, "-Puerta principal: ");
          strcat(mystring, texto_estado_puerta[digitalRead(sens_puerta)]);
          strcat(mystring, "\n");
          strcat(mystring, "-Ventana: ");
          strcat(mystring, texto_estado_puerta[sens_array[0].x]);
          strcat(mystring, " ");
          strcat(mystring, volt1);
          strcat(mystring, "V\n");
          strcat(mystring, "-Sensor de movimiento: ");
          strcat(mystring, volt2);
          strcat(mystring, "V");
          myBot.sendMessage(msg, mystring);
        }
      }
    }
  }
}
