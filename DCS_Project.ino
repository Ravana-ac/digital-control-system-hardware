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

static const int RXPin = D8, TXPin = D7;
static const uint32_t GPSBaud = 9600;

const byte ROWS = 4;
const byte COLS = 4;

String LAT, LON;

char keys[ROWS][COLS] = {
  { 'D', 'C', 'B', 'A' },
  { '#', '9', '6', '3' },
  { '0', '8', '5', '2' },
  { '*', '7', '4', '1' }
};

const char* keyMap[10] = {
  " _0",
  "ABC1",
  "DEF2",
  "GHI3",
  "JKL4",
  "MNO5",
  "PQR6",
  "STU7",
  "VWX8",
  "YZ.9",
};

byte rowPins[ROWS] = { 16, 0, 2, 14 };  //Keypad pins
byte colPins[COLS] = { 12, 3, 10, 9 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
WiFiClient wifiClient;
LiquidCrystal_I2C lcd(0x27, 16, 2);
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

bool wifiConnected = false;
bool setupCompleted = false;
bool transmitStarted = false;
bool gpsConnected = false;
bool manualWifi = false;

void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);
  init_lcd();

  connect_to_wifi();

  lcd.clear();
  delay(1000);

  gpsConnected = true;
}

unsigned long lastKeyPressTime = 0;
char currentKey = '\0';
int charIndex = 0;
String currentWord = "";

String line_0 = "";
String line_1 = "";

String p_line_0 = line_0;
String p_line_1 = line_1;

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

  if (!wifiConnected) {
    show_error(NO_WIFI);
    keypad_manager();
    update_lcd();
    return;
  }

  if (!setupCompleted) {
    line_0 = trainDataPrompts[inputId];
    keypad_manager();
    update_lcd();
    return;
  }

  if (transmitStarted && gpsConnected) {
    line_0 = "Transmitting....";
    get_location();
    send_request();
  }

  keypad_manager();
  update_lcd();
}

void show_error(String err) {
  line_0 = "     ERROR!";
  line_1 = err;
  update_lcd();
}

void send_request() {
  StaticJsonDocument<200> doc;
  doc["lat"] = LAT;
  doc["lon"] = LON;
  doc["train"] = trainDataInputs[0];
  doc["route"] = trainDataInputs[1];

  String jsonData;
  serializeJson(doc, jsonData);

  HTTPClient http;
  http.begin(wifiClient, "http://172.20.10.5:5000/update");
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(jsonData);

  if (httpResponseCode > 0) {
    line_1 = "OK " + String(httpResponseCode);
  } else {
    line_1 = "ERROR " + String(httpResponseCode);
  }
  update_lcd();

  http.end();
}

void connect_to_wifi() {

  byte temp_count = 0;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {

    if (temp_count % 2 == 0)
      lcd.print("Connecting...");
    else
      lcd.print("Connecting..");

    delay(1000);
    lcd.clear();
    temp_count++;

    if (temp_count > 15) {
      wifiConnected = false;
      return;
    }
  }

  wifiConnected = true;
}

void get_location() {
  LAT = String(gps.location.lat(), 6);
  LON = String(gps.location.lng(), 6);

  encord_gps(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    gpsConnected = false;
  }
}

void encord_gps(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (ss.available()) {
      gps.encode(ss.read());
    }
  } while (millis() - start < ms);
}

void init_lcd() {
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  lcd.print("  Initializing ");
  delay(1000);
  lcd.clear();
}

void capture_input(String input) {

  if (transmitStarted) {
    transmitStarted = false;
    wifiConnected = false;
    setupCompleted = false;
    inputId = 0;
    connect_to_wifi();
    return;
  }

  if (!setupCompleted) {
    if (inputId > INPUT_COUNT - 1) {
      setupCompleted = true;
      transmitStarted = true;
      inputId = 0;
      return;
    }
    trainDataInputs[inputId] = input;
    inputId++;
    return;
  }
}

void update_lcd() {
  if (p_line_0 != line_0 || p_line_1 != line_1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line_0);
    lcd.setCursor(0, 1);
    lcd.print(line_1);

    p_line_0 = line_0;
    p_line_1 = line_1;
  }
}
