#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <WebLED.h>
#include "html.h"

//#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
#define MAX_DEVICES 4
#define CS_PIN 15
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

#define ssid "Malang_SMKTelkom-2Ghz"
#define password "MokletHebat2021"

#define ledPin 2
#define led1_pin 5
#define led2_pin 4
#define led3_pin 0
bool ledState=0, led1 = 0, led2 = 0, led3 = 0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

//apa yang harus kita lakukan ketika ada pesan dari server
void notifyClients() {
  ws.textAll(
    String(ledState) + "|" + String(led1) + "|" + String(led2) + "|" + String(led3));

  digitalWrite(led1_pin, led1);
  digitalWrite(led2_pin, led2);
  digitalWrite(led3_pin, led3);
}

void on_event(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;

  for (; info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT;) {
    data[len] = 0;
    if (strcmp((char *)data, "toggle") == 0) {
      ledState = !ledState;
    } else if (strcmp((char *)data, "led1") == 0) {
      led1 = !led1;
    } else if (strcmp((char *)data, "led2") == 0) {
      led2 = !led2;
    } else if (strcmp((char *)data, "led3") == 0) {
      led3 = !led3;
    } else if (strcmp((char *)data, "status") == 0) {
    } else {
      break;
    }
    notifyClients();
    break;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);

  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);
  pinMode(led3_pin, OUTPUT);

  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);

  digitalWrite(ledPin, LOW);

  start_wifi(ssid, password);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  initWebSocket();

  myDisplay.begin();

  myDisplay.setIntensity(10);
  myDisplay.displayClear();
  //text display
  myDisplay.displayScroll("SMK Telkom Malang", PA_CENTER, PA_SCROLL_RIGHT, 100);
  myDisplay.setZoneEffect(0, true, PA_FLIP_LR);
  myDisplay.setZoneEffect(0, true, PA_FLIP_UD);
}

void loop() {
  ws.cleanupClients();

  digitalWrite(ledPin, ledState);

  if (myDisplay.displayAnimate()) {
    myDisplay.displayReset();
  }
}

void initWebSocket() {
  server.addHandler(&ws);
  ws.onEvent(onEvent);
  server.begin();
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      on_event(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}
