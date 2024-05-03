#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

WebServer server(8080);

void handleRoot()
{   server.send(200, "text/plain", "Alarma Activada ;)");
    if (estado_alarma == true)
    {
        isopen = true;
    }
}

void setup_server()
{
    if (MDNS.begin("esp32_alarma"))
    {
        Serial.println("MDNS started");
    }
    delay(150);
    server.begin();
    Serial.println("HTTP server started");
    server.on("/", handleRoot);
}
