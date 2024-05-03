#include <WiFi.h>
#include <WiFiClientSecure.h>
WiFiClientSecure secured_client;
const char *ssid = ;
const char *password = ;

#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define BOT_TOKEN 
#define CHAT_ID 

UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void wifi_connect()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

  Serial.println("");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");

    pixels.setPixelColor(0, 90, 0, 255);
    pixels.show();
    delay(125);
    pixels.clear();
    pixels.show();
    delay(125);
  }
  Serial.println("Wifi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void keep_wifi_alive()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    WiFi.disconnect();
    delay(1000);
    wifi_connect;
  }
}
