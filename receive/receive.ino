#include "wifi_config.h"
#include <wifi_manager.h>
#include <WebServer.h>
#include <ota_manager.h>


#define LED_PIN 18

WebServer server(80);


bool ledState = false;
bool blinking = false;

unsigned long occupiedStart = 0;
unsigned long lastBlink = 0;
bool blinkState = false;

// const unsigned long BLINK_AFTER = 30UL * 60UL * 1000UL;  //30分
const unsigned long BLINK_AFTER = 10000;  //10秒

void handleLed() {
  Serial.println("Received!");
  if (!server.hasArg("state")) {
    server.send(400, "text/plain", "Missing state");
    return;
  }

  bool state = server.arg("state").toInt();

  Serial.print("state = ");
  Serial.println(state);


  server.send(200, "text/plain", "OK");


  if (state != ledState) {
    ledState = state;

    if (ledState) {
      occupiedStart = millis();
      blinking = false;
    } else {
      blinking = false;
      digitalWrite(LED_PIN, LOW);
    }
  }
  Serial.print("state = ");
  Serial.println(state);

  Serial.print("ledState = ");
  Serial.println(ledState);
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

  if (ledState) {

    if (!blinking && millis() - occupiedStart >= BLINK_AFTER) {
      blinking = true;
      blinkState = true;
    }

    if (blinking) {
      if (millis() - lastBlink >= 500) {
        lastBlink = millis();
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);
      }
    } else {
      digitalWrite(LED_PIN, HIGH);
    }
  }
}