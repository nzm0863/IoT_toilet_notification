#include "wifi_config.h"
#include <wifi_manager.h>
#include <WebServer.h>
#include <ota_manager.h>


#define LED_PIN 18

WebServer server(80);

void handleLed() {
  Serial.println("Received!");
  if (!server.hasArg("state")) {
    server.send(400, "text/plain", "Missing state");
    return;
  }

  bool state = server.arg("state").toInt();

  Serial.print("state = ");
  Serial.println(state);

  digitalWrite(LED_PIN, state ? HIGH : LOW);

  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  wifiConnect(WIFI_SSID, WIFI_PASSWORD);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  otaSetup("toilet_receive");


  server.on("/led", handleLed);
  server.begin();
}

void loop() {
  otaHandle();
  server.handleClient();
}