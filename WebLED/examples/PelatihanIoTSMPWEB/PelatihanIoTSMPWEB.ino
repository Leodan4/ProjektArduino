#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <WebLED.h>

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

//#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW

#define MAX_DEVICES 4
#define CS_PIN 15

MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

const char *ssid = "Malang_SMKTelkom-2Ghz";
const char *password = "MokletHebat2021";

int ledState = 0;
#define ledPin 2
#define led1_pin 5
#define led2_pin 4
#define led3_pin 0
bool led1 = 0, led2 = 0, led3 = 0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void notifyClients() {
  ws.textAll(
    String(ledState) + "|" + String(led1) + "|" + String(led2) + "|" + String(led3));

  digitalWrite(led1_pin, led1);
  digitalWrite(led2_pin, led2);
  digitalWrite(led3_pin, led3);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  //rpl warcrime
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

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup() {
  Serial.begin(115200);

  // pinMode(testtPin, INPUT);
  pinMode(ledPin, OUTPUT);

  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);
  pinMode(led3_pin, OUTPUT);

  digitalWrite(ledPin, LOW);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  // Start server
  server.begin();
  myDisplay.begin();

  // Set the intensity (brightness) of the display (0-15)
  myDisplay.setIntensity(10);
  myDisplay.displayClear();
  myDisplay.displayScroll("SMK Telkom Malang", PA_CENTER, PA_SCROLL_LEFT, 75);
}

void loop() {
  ws.cleanupClients();

  digitalWrite(ledPin, ledState);

  if(myDisplay.displayAnimate()){
    myDisplay.displayReset();
  }
}
