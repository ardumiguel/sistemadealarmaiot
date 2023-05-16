unsigned long t_buzzer = 0;
unsigned int t = 0;
unsigned int z = 0;


int melody_enable[3] = { 2093, 2637, 3135 };
int melody_disable[3] = { 3135, 2637, 2093 };


void buzzer_update() {
  if (millis() - t_buzzer >= t) {
    ledcDetachPin(z);
  }
}
void buzzer_tone(int pin, unsigned int freq, unsigned int duration) {
  ledcSetup(0, 500, 8);
  ledcAttachPin(pin, 0);
  z = pin;
  t = duration;
  t_buzzer = millis();

  ledcWriteTone(0, freq);
  ledcWrite(0, 250);
}


void play_melody(int n) {
  static int count = 0;
  static int t_antes = 0;

  if (n == 1) {

    while (count < 3) {
      if (millis() - t_antes >= 85 && count < 3) {
        buzzer_tone(buzzer, melody_enable[count], 75);
        count++;
        t_antes = millis();
      }
      if (millis() - t_buzzer >= 75) {
        ledcDetachPin(z);
      }

      if (count == 3) {
        count = 0;
        return;
      }
    }

  }

  else if (n == 2) {
    while (count < 3) {

      if (millis() - t_antes >= 85 && count < 3) {
        buzzer_tone(buzzer, melody_disable[count], 75);
        count++;
        t_antes = millis();
      }
      if (millis() - t_buzzer >= 75) {
        ledcDetachPin(z);
      }

      if (count == 3) {
        count = 0;
        return;
      }
    }
  }

  else if (n == 0) {

    while (count < 2) {
      if (millis() - t_antes >= 85) {
        buzzer_tone(buzzer, 400, 70);
        count += 1;
        t_antes = millis();
      }
      if (millis() - t_buzzer >= 70) {
        ledcDetachPin(z);
      }

      if (count == 2) {
        count = 0;
        return;
      }
    }
  }
}