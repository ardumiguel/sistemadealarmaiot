#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 22  // Pin 22 para el reset del RC522
#define SS_PIN 17   // Pin 5 para el SS (SDA) del RC522
/*
  sda/ss -> 17
  sck -> 18
  mosi -> 23
  miso -> 19
  rst -> 22
*/

MFRC522 rfid(SS_PIN, RST_PIN);
long nuidsum;

void RFID_setup() {

  SPI.begin();      // Iniciamos el Bus SPI
  rfid.PCD_Init();  // Iniciamos  el MFRC522
  rfid.PCD_SetAntennaGain(0x07 << 4);
  Serial.println("RFID init");
}

void RFID_update() {
  // si hay nuevas tarjetas  presentes

  if (estado_sistema != prev_estado_sistema) {
    if (estado_sistema >= 1)
      rfid.PCD_SoftPowerUp();
    else if (estado_sistema == 0)
      rfid.PCD_SoftPowerDown();

    /*AQUI NO IGUALO PREV Y ACTUAL PORQUE YA SUCEDE EN G_DATA_STORAGE*/
  }

  if (estado_sistema > 1) {
    if (rfid.PICC_IsNewCardPresent()) {
      if (rfid.PICC_ReadCardSerial()) {

        nuidsum = 0;
        for (int i = 0; i < 4; i++) {
          nuidsum += rfid.uid.uidByte[i];
        }
        Serial.print("RFID Detectado, nuidsum -> ");
        Serial.println(nuidsum);
        rfid.PICC_HaltA();
      }

      if (nuidsum == 10 || nuidsum == 348 || nuidsum == 465) {
        nuidsum = 0;
        estado_sistema = 0;
        erase_code();
        play_melody(2);
        for (int i = 0; i < 8; i++)
          set_led_color(i, 1000, 0, 0, 0);

      } else {
        play_melody(0);
      }
    }
  }
}






/*ruit_Fingerprint.h>


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);
TaskHandle_t Task1;

void fp_init() {

  finger.begin(57600);
  delay(100);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    finger.LEDcontrol(0);
  } else {
    Serial.println("Did not find fingerprint sensor");
  }




void core2func(void* parameters) {
    fp_main();
  }
}

void fp_main() {

  static bool hasrun;

  if (estado_sistema >= 2 && fp_reader == true) {

    // 0 hay dedo
    // 2 no hay dedo


    uint8_t fprint = finger.getImage();

    if (fprint == FINGERPRINT_NOFINGER)
      hasrun = false;

    if (fprint == FINGERPRINT_OK && hasrun == false) {

      hasrun = true;
      uint8_t p;
      set_led_color(150, 255, 255, 255);
      buzzer_tone(buzzer, 4186, 70);
      p = fprint;
      p = finger.image2Tz();

      p = finger.fingerSearch(1);
      if (p != FINGERPRINT_OK)
        p = finger.fingerSearch(2);

      set_led_color(150, 0, 255, 0);

      if (p != FINGERPRINT_OK) {
        //Serial.println("Error de lectura");
        play_melody(0);
      } else {

        estado_sistema = 0;
        erase_code();
        set_led_color(300, 0, 0, 0);
        play_melody(2);
        // found a match!
        //Serial.print("Huella ID #");
        //Serial.print(finger.fingerID);
        //Serial.print(" con nivel de confianza ");
        //Serial.println(finger.confidence);
      }
    }
  } else {

    finger.LEDcontrol(0);
  }
}*/