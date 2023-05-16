#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

static esp_err_t init_camera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 15000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 5;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_count = 2;


  Serial.print("Camera init... ");
  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    delay(100);  // need a delay here or the next serial o/p gets missed
    Serial.printf("\n\nCRITICAL FAILURE: Camera sensor failed to initialise.\n\n");
    Serial.printf("A full (hard, power off/on) reboot will probably be needed to recover from this.\n");
    ESP.restart();
    return err;
  } else {
    Serial.println("succeeded");

    // Get a reference to the sensor
    sensor_t* s = esp_camera_sensor_get();

    // Dump camera module, warn for unsupported modules.
    switch (s->id.PID) {
      case OV9650_PID: Serial.println("WARNING: OV9650 camera module is not properly supported, will fallback to OV2640 operation"); break;
      case OV7725_PID: Serial.println("WARNING: OV7725 camera module is not properly supported, will fallback to OV2640 operation"); break;
      case OV2640_PID: Serial.println("OV2640 camera module detected"); break;
      case OV3660_PID: Serial.println("OV3660 camera module detected"); break;
      default: Serial.println("WARNING: Camera module is unknown and not properly supported, will fallback to OV2640 operation");
    }
  }

  fb = esp_camera_fb_get();
  esp_camera_fb_return(fb);
  fb = NULL;
  return ESP_OK;
}
void takepicture() {
  Serial.println("capturing photo");
  digitalWrite(LAMP_PIN, HIGH);
  fb = esp_camera_fb_get();
  digitalWrite(LAMP_PIN, LOW);
  t_antes = millis();

  if (fb) {
    Serial.println("Camera capture success");
  } else {
    myBot.sendTo(CHAT_ID, "Error de cámara");
    Serial.println("Camera capture failed");
    ESP.restart();
  }
}