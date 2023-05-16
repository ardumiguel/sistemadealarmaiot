
const char* ssid = "SSID";
const char* password = "PASSWORD";

IPAddress ip(192, 168, 1, 163);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
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
  static bool hasrun = false;

  WiFi.mode(WIFI_AP_STA);

  //WiFi.config(ip, gateway, subnet, dns);
  WiFi.begin(ssid, password);


  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");

    pixels.setPixelColor(0, 90, 0, 255);
    pixels.show();
    delay(125);
    pixels.clear();
    pixels.show();
    delay(125);

    count++;
    if (count > 50) {
      count = 0;
      return;
    }
  }
  count = 0;



  /*Casi lo mismo que WiFi.printDiag(Serial)*/
  Serial.println("Wifi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Channel: ");
  Serial.println(WiFi.channel());
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.println("");


  /*TELEGRAM - Lo configuramos sólo cuando se ejecuta por primera vez*/
  if (hasrun == false) {
    configTzTime(MYTZ, "time.google.com", "time.windows.com", "pool.ntp.org");
    client.setCACert(telegram_cert);
    myBot.setUpdateTime(2000);
    myBot.setTelegramToken(BOT_TOKEN);

    kbd_1.addButton("Solo puertas", "puertas", KeyboardButtonQuery);
    kbd_1.addButton("Completo", "completo", KeyboardButtonQuery);

    kbd_2.addButton("Sí", "si", KeyboardButtonQuery);
    kbd_2.addButton("No", "no", KeyboardButtonQuery);
    hasrun = true;
    /*if (myBot.begin())
      Serial.println("Conexion con telegram establecida");*/
  }
}

void wifi_keep_alive() { /*pregunta cada 5 min si hay una conex wifi*/
  static unsigned long t_antes;
  if (millis() - t_antes >= 300000 && WiFi.status() != WL_CONNECTED) {
    wifi_connect();
    t_antes = millis();
  }
}



void telegram_update() {

  if (WiFi.status() == WL_CONNECTED) {
    if (myBot.getNewMessage(msg)) {
      if (msg.chatId == CHAT_ID) {

        //Messagequery//
        if (msg.messageType == MessageQuery) {
          if (msg.callbackQueryData.equals("puertas")) {
            myBot.endQuery(msg, "Alarma activada en el modo Solo puertas");
            myBot.sendTo(CHAT_ID, "Alarma activada en el modo Solo puertas");
            estado_sistema = 2;
            erase_code();
            play_melody(1);
            set_led_color(300, 0, 255, 0);

          } else if (msg.callbackQueryData.equals("completo")) {
            myBot.endQuery(msg, "Alarma activada en el modo Completo");
            myBot.sendTo(CHAT_ID, "Alarma activada en el modo Completo");
            estado_sistema = 3;
            erase_code();
            play_melody(1);
            set_led_color(300, 0, 255, 0);

          } else if (msg.callbackQueryData.equals("si")) {
            myBot.endQuery(msg, "Selecciona un modo:");
            myBot.sendMessage(msg, "Selecciona un modo:", kbd_1);

          } else if (msg.callbackQueryData.equals("no")) {
            myBot.endQuery(msg, "Recibido");
            myBot.sendTo(CHAT_ID, "Recibido");
          }
        }  //Messagequery//

        if (msg.text == "/foto") {
          structure senddata{ 0, false, 0 };  //id=0, x=true
          esp_now_send(espcamaddress, (uint8_t*)&senddata, sizeof(senddata));
          senddata = { 0, true, 0 };  //id=0, x=true
          esp_now_send(espcamaddress, (uint8_t*)&senddata, sizeof(senddata));
        }

        if (msg.text == "/bateria") {
          char mystring[80];
          char volt1[5];
          char volt2[5];
          dtostrf(sens_array[0].volt, 1, 2, volt1);
          dtostrf(sens_array[1].volt, 1, 2, volt2);

          mystring[0] = '\0';  //strcat empieza a concatenar en el null termination
          strcat(mystring, "Niveles de batería más recientes\n");
          strcat(mystring, "Sensor ventana: ");
          strcat(mystring, volt1);
          strcat(mystring, "V\n");
          strcat(mystring, "Sensor de movimiento: ");
          strcat(mystring, volt2);
          strcat(mystring, "V\n");
          myBot.sendTo(CHAT_ID, mystring);
        }


        if (msg.text == "/desactivaralarma") {
          if (estado_sistema == 0)
            myBot.sendTo(CHAT_ID, "La alarma ya está desactivada");

          else {
            estado_sistema = 0;
            erase_code();
            play_melody(2);
            set_led_color(300, 0, 0, 0);
            myBot.sendTo(CHAT_ID, "Alarma desactivada correctamente");
          }
        }

        if (msg.text == "/activaralarma") {

          if (estado_sistema >= 2)

            myBot.sendTo(CHAT_ID, "La alarma ya está activada");
          else if ((digitalRead(sens_puerta) + sens_array[0].x) != 0) {
            myBot.sendMessage(msg, "Hay alguna puerta o ventana abierta\n¿Desea continuar?", kbd_2);
          } else {
            myBot.sendMessage(msg, "Selecciona un modo:", kbd_1);
          }
        }

        if (msg.text == "/informacion") {
          char mystring[150];
          mystring[0] = '\0';  //strcat empieza a concatenar en el null termination
          strcat(mystring, "\xF0\x9F\x8F\xA0 INFORMACIÓN DEL SISTEMA \xF0\x9F\x8F\xA0 \n\n");
          strcat(mystring, "Estado de la alarma: ");
          if (isopen == false)
            strcat(mystring, texto_estado_sistema[estado_sistema]);
          else
            strcat(mystring, texto_isopen_reason[isopen_reason]);
          strcat(mystring, "\n");
          strcat(mystring, "Sensores:\n");
          strcat(mystring, " -Puerta principal: ");
          strcat(mystring, texto_estado_puerta[digitalRead(sens_puerta)]);
          strcat(mystring, "\n");
          strcat(mystring, " -Ventana: ");
          strcat(mystring, texto_estado_puerta[sens_array[0].x]);
          myBot.sendTo(CHAT_ID, mystring);
        }
      }
    }
  }
}
