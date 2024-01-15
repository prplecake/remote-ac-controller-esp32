#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "Credentials.h"

WebServer server(80);

#define LED 13

/*
 * Handle the root path.
 */
void handleRoot() {
    digitalWrite(LED, HIGH);
    server.send(200, "text/plain", "hello from esp32!");
    digitalWrite(LED, LOW);
}

/*
 * Handle the 404 path.
 */
void handleNotFound() {
    digitalWrite(LED, HIGH);
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
    digitalWrite(LED, LOW);
}

__attribute__((unused)) void setup( )
{
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
#ifdef WIFI_HOSTNAME
    Serial.print("Setting hostname to ");
    Serial.println(WIFI_HOSTNAME);
    WiFi.setHostname(WIFI_HOSTNAME);
#else
    Serial.print("Hostname not configured. Default hostname: ");
    Serial.println(WiFi.getHostname());
#endif
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(WIFI_SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("esp32")) {
        Serial.println("MDNS responder started");
    }

    server.on("/", handleRoot);

    server.on("/inline", []() {
        server.send(200, "text/plain", "this works as well");
    });

    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");
}

/*
 * Main program loop.
 */
__attribute__((unused)) void loop( )
{
    server.handleClient();
    delay(2);  // Allow the CPU to switch to other tasks
}
