



// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&recvdata, incomingData, sizeof(recvdata));



  /*ALMACENADO DE LOS DATOS*/
  sens_array[recvdata.id - 1].x = recvdata.x;
  sens_array[recvdata.id - 1].volt = recvdata.volt;
  //Serial.println(recvdata.volt);

  /*TRATAMIENTO DE LOS DATOS*/
  if (recvdata.volt <= 2.8 && recvdata.volt != 0) {
    char mystring[80];
    mystring[0] = '\0';  //strcat empieza a concatenar en el null termination
    strcat(mystring, "Batería baja en el sensor de ");
    if (recvdata.id == 1)
      strcat(mystring, "ventana");
    if (recvdata.id == 2)
      strcat(mystring, "movimiento");
    myBot.sendTo(CHAT_ID, mystring);
  }

  /*
  El isopen==false es para respetar el isopen_reason y que 
  veraderamente le corresponda el id que hace saltar la alarma
  ya que pueden llegar otros ids despues
  */

  if (isopen == false && estado_sistema == 3) { /*Modo completo*/
    isopen_reason = recvdata.id;
    switch (recvdata.id) {
      case 0:
        isopen = true;
        break;
      default:
        if (recvdata.x == 1)
          isopen = true;
        break;
    }
  } else if (isopen == false && estado_sistema == 2 && recvdata.id <= 1 && recvdata.x == 1) { /*Modo solo puertas*/
    isopen_reason = recvdata.id;
    isopen = true;
  }
}
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void espnow_init() {
  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  memcpy(peerInfo.peer_addr, espcamaddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);
  Serial.println("ESPNOW init");
}
