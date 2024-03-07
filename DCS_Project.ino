#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

#include <floatToString.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define NO_WIFI "100"
#define NO_GPS "101"
#define INET_ERR "102"

#define WIFI_SSID "Sanju's iPhone"
#define WIFI_PASSWORD "12345678"
#define WIFI_ATTEMPT_COUNT 10
#define REQUEST_WAIT_TIME 1000

static const int RXPin = D8, TXPin = D7;
static const uint32_t GPSBaud = 9600;

WiFiClient wifiClient;
LiquidCrystal_I2C lcd(0x27, 16, 2);
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
StaticJsonDocument<1024> doc;

WebSocketsClient webSocket;

enum SystemState {
  STATE_CONNECTING_WIFI,
  STATE_CONNECED_WIFI,
  STATE_NO_WIFI,
  STATE_NO_GPS,
  STATE_SETUP,
  STATE_SETUP_COMPLETED,
  STATE_TRANSMITTING,
  STATE_IDLE
};

SystemState currentState = STATE_CONNECTING_WIFI;

void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);
  initLCD();

  currentState = connectToWIFI();

  lcd.clear();
  delay(1000);

  // Setup WebSocket connection
  webSocket.begin("172.20.10.5", 5000, "/");  // Use SSL (wss) for secure connection: webSocket.beginSSL("example.com", 443, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);  // Try to reconnect every 5 seconds
}


String line_0 = "";
String line_1 = "";

String p_line_0 = line_0;
String p_line_1 = line_1;

String LAT, LON;

int inputId = 0;
const byte INPUT_COUNT = 2;
String trainDataPrompts[INPUT_COUNT] = {
  "Train Name:",
  "Route Number:"
};

String trainDataInputs[INPUT_COUNT] = {
  "TRAIN_ID",
  "ROUTE_ID"
};

long requestDelay = millis();

void loop() {
  webSocket.loop();
  switch (currentState) {
    case STATE_CONNECTING_WIFI:
      currentState = connectToWIFI();
      break;

    case STATE_CONNECED_WIFI:
      currentState = STATE_SETUP;
      break;

    case STATE_NO_WIFI:
      showError(NO_WIFI);
      break;

    case STATE_SETUP:
      line_0 = trainDataPrompts[inputId];
      break;

    case STATE_SETUP_COMPLETED:
      currentState = STATE_TRANSMITTING;
      break;

    case STATE_NO_GPS:
      showError(NO_GPS);
      break;

    case STATE_TRANSMITTING:
      line_0 = "Transmitting....";
      if (millis() - requestDelay > REQUEST_WAIT_TIME) {
        getLocation();
        sendRequest();
        requestDelay = millis();
      }
      break;

    case STATE_IDLE:
      line_0 = "      Idle";
      break;
  }

  keypadManager();
  updateLCD();
}

void showError(String err) {
  line_0 = "     ERROR!";
  line_1 = err;
  updateLCD();
}

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      break;
    case WStype_CONNECTED:

      break;
    case WStype_TEXT:
      if(currentState == STATE_TRANSMITTING){
        line_1 = String((char*)payload);
        updateLCD();
      }
      break;
  }
}

void sendRequest() {

  doc["lat"] = LAT;
  doc["lon"] = LON;
  doc["route"] = trainDataInputs[1];

  String jsonData;
  serializeJson(doc, jsonData);
  webSocket.sendTXT(jsonData);

}

SystemState connectToWIFI() {

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  for (int i = 0; i < WIFI_ATTEMPT_COUNT; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      return STATE_CONNECED_WIFI;
    }

    if (i % 2 == 0)
      lcd.print("Connecting...");
    else
      lcd.print("Connecting..");

    delay(1000);
    lcd.clear();
  }
  return STATE_NO_WIFI;
}

void getLocation() {
  LAT = String(gps.location.lat(), 6);
  LON = String(gps.location.lng(), 6);

  encordGPS(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    currentState = STATE_NO_GPS;
  }
}

void encordGPS(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (ss.available()) {
      gps.encode(ss.read());
    }
  } while (millis() - start < ms);
}

void initLCD() {
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  lcd.print("  Initializing ");
  delay(1000);
  lcd.clear();
}

void captureInput(String input) {

  switch (currentState) {
    case STATE_SETUP:
      if (inputId >= INPUT_COUNT) {
        currentState = STATE_SETUP_COMPLETED;
        inputId = 0;
      } else {
        trainDataInputs[inputId] = input;
        inputId++;
      }
      break;

    case STATE_TRANSMITTING:
      break;
    case STATE_CONNECTING_WIFI:
      break;
    case STATE_IDLE:
      currentState = STATE_CONNECTING_WIFI;
      break;
  }
}
