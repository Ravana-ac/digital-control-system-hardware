#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
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

static const int RXPin = D8, TXPin = D7;
static const uint32_t GPSBaud = 9600;

WiFiClient wifiClient;
LiquidCrystal_I2C lcd(0x27, 16, 2);
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
StaticJsonDocument<200> doc;

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

bool wifiConnected = false;
bool setupCompleted = false;
bool transmitStarted = false;
bool gpsConnected = false;

SystemState currentState = STATE_CONNECTING_WIFI;

void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);
  initLCD();

  currentState = connectToWIFI();

  lcd.clear();
  delay(1000);

  gpsConnected = true;
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
      getLocation();
      sendRequest();
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

void sendRequest() {

  const String url = "http://172.20.10.5:5000/" + trainDataInputs[0];

  doc["lat"] = LAT;
  doc["lon"] = LON;
  doc["route"] = trainDataInputs[1];

  String jsonData;
  serializeJson(doc, jsonData);

  HTTPClient http;
  http.begin(wifiClient, url.c_str());
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(jsonData);

  if (httpResponseCode > 0) {
    if (httpResponseCode == 404) {
      line_1 = "Invalid Train ID";
    } else {
      line_1 = String(httpResponseCode);
    }
  } else {
    line_1 = String(httpResponseCode) + " ERROR";
  }
  updateLCD();

  http.end();
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
