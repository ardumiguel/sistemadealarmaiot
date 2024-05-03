#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 21 // Pin 21 para el reset del RC522
#define SS_PIN 5   // Pin 5 para el SS (SDA) del RC522
                   /*
                   sda/ss -> 5
                   sck -> 18
                   mosi -> 23
                   miso -> 19
                   rst -> 22
                   */

MFRC522 rfid(SS_PIN, RST_PIN);
long nuid;

void RFID_setup()
{

    SPI.begin();     // Iniciamos el Bus SPI
    rfid.PCD_Init(); // Iniciamos  el MFRC522
    Serial.println("RFID init");
}

void RFID_update()
{
    // si hay nuevas tarjetas  presentes
    if (rfid.PICC_IsNewCardPresent())
    {
        if (rfid.PICC_ReadCardSerial())
        {
            nuid = rfid.uid.uidByte[2];
            Serial.println("rfid");
            rfid.PICC_HaltA();
        }

        if (estado_alarma == true && nuid == 225 || estado_alarma == true && nuid == 63)
        {
            estado_alarma = !estado_alarma;

            pixels.setPixelColor(0, 0, 0, 0);
            pixels.show();
            
            play_melody(2);
        }
    }
}