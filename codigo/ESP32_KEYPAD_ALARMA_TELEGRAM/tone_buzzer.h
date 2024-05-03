unsigned long t_buzzer = 0;
unsigned int t = 0;
unsigned int z = 0;

void buzzer_update() {
  if (millis() - t_buzzer >= t)
  {
    ledcDetachPin(z);
  }
}
void buzzer_tone(int pin, unsigned int freq, unsigned int duration)
{
  ledcSetup(0, 500, 8);
  ledcAttachPin(pin, 0);
  z = pin;
  t = duration;
  t_buzzer = millis();
  ledcWriteTone(0, freq);
}