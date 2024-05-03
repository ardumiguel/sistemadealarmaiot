#include <PubSubClient.h>

const char *mqtt_server = "192.168.1.150";
const int port = 1883;

PubSubClient client(mqtt_server, port, espClient);

void mqtt_connect()
{

  String Id = "ESP32Client-";
  Id += String(random(0xffff), HEX);
  // Attempt to connect

  while (!client.connected())
  {
    
    Serial.print(".");
    delay(500);
    if (client.connect(Id.c_str()))
    {
      Serial.println("MQTT connected");
    }
  }
}

void keep_mqtt_alive()
{
  if (!client.connected())
  {
    delay(500);
    mqtt_connect();
  }
}

void mqtt_send(const char topic[], const char payload[])
{
  client.publish(topic, payload);
}
