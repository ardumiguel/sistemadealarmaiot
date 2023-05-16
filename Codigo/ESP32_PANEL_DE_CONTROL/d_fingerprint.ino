#include <Adafruit_Fingerprint.h>


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

  xTaskCreatePinnedToCore(
    core2func, /* Task function. */
    "Task1",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Task1,    /* Task handle to keep track of created task */
    0);        /* pin task to core 0 */
  delay(500);
}


void core2func(void* parameters) {
  for (;;) { /*como si fuera void loop*/
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
}