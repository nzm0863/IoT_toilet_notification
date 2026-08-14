#include "wifi_config.h"
#include <wifi_manager.h>
#include <ota_manager.h>
#include <HTTPClient.h>
#include <WiFi.h>
#define SENSOR_PIN 34

void setup() {
  Serial.begin(115200);

  wifiConnect(WIFI_SSID, WIFI_PASSWORD);
  otaSetup("toilet_send");
}

int arr[3] = { 0, 0, 0 };
int count = 0;
unsigned long lastTime = 0;
const char* LED_SERVER = "http://toilet_receive.local";
bool lastOccupied = false;

unsigned long lastSend = 0;
const unsigned long SEND_INTERVAL = 30000;  //30秒
unsigned long lastWifiCheck = 0;

void loop() {
  otaHandle();
  if (millis() - lastWifiCheck >= 10000) {
    lastWifiCheck = millis();

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected. Reconnecting...");
      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
  }
  if (millis() - lastTime >= 1000) {
    lastTime = millis();
    int lightValue = analogRead(SENSOR_PIN);
    Serial.println(lightValue);

    int threshold = 2800;


    // 最新の値を保存
    arr[count] = lightValue;
    count = (count + 1) % 3;

    // 全部しきい値以上か判定
    bool allHigh = true;

    for (int i = 0; i < 3; i++) {
      if (arr[i] < threshold) {
        allHigh = false;
        break;
      }
    }

    bool occupied = !allHigh;

    if (occupied != lastOccupied || millis() - lastSend >= SEND_INTERVAL) {

      lastOccupied = occupied;
      lastSend = millis();

      String url = String(LED_SERVER) + "/led?state=" + String(occupied);

      HTTPClient http;
      http.begin(url);
      int httpCode = http.GET();

      if (httpCode != 200) {
        Serial.printf("HTTP Error: %d\n", httpCode);
      }

      http.end();
    }
  }
}