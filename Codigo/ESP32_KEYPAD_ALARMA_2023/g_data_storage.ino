
void data_init() {
  preferences.begin("system", false);
  estado_sistema = preferences.getInt("mode", 0);
  isopen = preferences.getBool("isopenstat", 0);
  pass[0] = preferences.getChar("storedpass0", '1');
  pass[1] = preferences.getChar("storedpass1", '2');
  pass[2] = preferences.getChar("storedpass2", '3');
  pass[3] = preferences.getChar("storedpass3", '4');

  if (estado_sistema >= 2) {
    set_led_color(0, 700, 0, 255, 0);
  } else if (estado_sistema == 1) {
    estado_sistema = 2;
    set_led_color(0, 700, 0, 255, 0);
  }
}

void store_data() {
  static int prev_isopen;

  if (estado_sistema != prev_estado_sistema || isopen != prev_isopen) {
    preferences.putInt("mode", estado_sistema);
    preferences.putBool("isopenstat", isopen);
    prev_estado_sistema = estado_sistema;
    prev_isopen = isopen;
  }
}