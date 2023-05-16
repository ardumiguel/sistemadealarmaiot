
int fps = 25;

struct colorvalue {
  int r;
  int g;
  int b;
};

colorvalue target_color;
colorvalue prev_color;
colorvalue constant;

int get_constant(byte color, byte prev_col, int target_time) {
  return abs(color - prev_col) * fps / target_time;
}

void set_led_color(int t, byte r_, byte g_, byte b_) {

  target_color = { r_, g_, b_ };

  constant = { get_constant(r_, prev_color.r, t),
               get_constant(g_, prev_color.g, t),
               get_constant(b_, prev_color.b, t) };


  /*Para cuando la constante sale por debajo de 0*/
  if ((r_ - prev_color.r) != 0 && constant.r == 0)
    constant.r = 1;
  if ((g_ - prev_color.g) != 0 && constant.g == 0)
    constant.g = 1;
  if ((b_ - prev_color.b) != 0 && constant.b == 0)
    constant.b = 1;
}

void led_update() {
  static unsigned long t_antes = 0;

  if (prev_color.r != target_color.r || prev_color.g != target_color.g || prev_color.b != target_color.b) {

    if (millis() - t_antes >= 25) {

      /*RED*/
      if (target_color.r > prev_color.r) {
        prev_color.r += constant.r;
        prev_color.r = constrain(prev_color.r, 0, target_color.r);
      } else {
        prev_color.r -= constant.r;
        prev_color.r = constrain(prev_color.r, target_color.r, 255);
      }

      /*GREEN*/

      if (target_color.g > prev_color.g) {
        prev_color.g += constant.g;
        prev_color.g = constrain(prev_color.g, 0, target_color.g);
      } else {
        prev_color.g -= constant.g;
        prev_color.g = constrain(prev_color.g, target_color.g, 255);
      }

      /*BLUE*/
      if (target_color.b > prev_color.b) {
        prev_color.b += constant.b;
        prev_color.b = constrain(prev_color.b, 0, target_color.b);
      } else {
        prev_color.b -= constant.b;
        prev_color.b = constrain(prev_color.b, target_color.b, 255);
      }


      pixels.setPixelColor(0, prev_color.r, prev_color.g, prev_color.b);
      pixels.show();

      /*Serial.print(constant.r);
      Serial.print("/");
      Serial.print(ir);
      Serial.print("    ");
      Serial.print(constant.g);
      Serial.print("/");
      Serial.print(prev_color.g);
      Serial.print("    ");
      Serial.print(constant.b);
      Serial.print("/");
      Serial.print(ib);
      Serial.println("");*/

      t_antes = millis();
    }
  }
}
